#include "pic.h"
#include <x86_64/cpu.h>

//
// CPU 8259 programmable interrupt controller driver implementation
//

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define PIC_EOI 0x20

void RemapPIC(uint8_t offset1, uint8_t offset2)
{
	// ICW1
	x86_outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // Master PIC
	x86_iowait();
	x86_outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4); // Slave PIC
	x86_iowait();

	// ICW2
	x86_outb(PIC1_DATA, offset1); // Master PIC offset
	x86_iowait();
	x86_outb(PIC2_DATA, offset2); // Slave PIC offset
	x86_iowait();

	// ICW3
	x86_outb(PIC1_DATA, 1 << 2); // Tell master PIC that there's a slave PIC at IRQ2
	x86_iowait();
	x86_outb(PIC2_DATA, 2); // Tell slave PIC its identity (0000 0010)
	x86_iowait();

	// ICW4
	x86_outb(PIC1_DATA, ICW4_8086); // 8086 mode instead of 8080 mode
	x86_iowait();
	x86_outb(PIC2_DATA, ICW4_8086);
	x86_iowait();

	// Unmask all IRQs
	x86_outb(PIC1_DATA, 0);
	x86_outb(PIC2_DATA, 0);
}

void PIC_SendEOI(int irq)
{
	if (irq >= 8)
	{
		x86_outb(PIC2_COMMAND, PIC_EOI);
	}

	x86_outb(PIC1_COMMAND, PIC_EOI);
}


