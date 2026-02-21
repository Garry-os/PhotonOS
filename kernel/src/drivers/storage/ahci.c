//
// Small AHCI controller driver
// For reading disk
// Implemented using SATA AHCI spec revision 1.3.1
//
#include "ahci.h"
#include <vmm.h>
#include <paging.h>
#include <qemu/print.h>
#include <utils/memory.h>
#include <malloc.h>
#include <x86_64/timer.h>

// CAP2
#define AHCI_BIOS_HANDOFF (1 << 0) // If the BIOS/OS handoff mechanism is supported

// BOHC
#define AHCI_OOS (1 << 1) // OS ownership
#define AHCI_BOS (1 << 0) // BIOS ownership
#define AHCI_BB  (1 << 4) // BIOS busy

// GHC
#define AHCI_HR (1 << 0) // HBA reset
#define AHCI_AE (1 << 31) // AHCI enable

void InitAHCI(PCIDevice* device)
{
	PCIGeneralHeader general = device->generalHeader;

	uint32_t commandStatus = (uint32_t)((device->header.status << 16 | device->header.command));

	// Enable PCI bus mastering, memory space access
	commandStatus |= PCI_BUS_MASTERING;
	commandStatus |= PCI_MEMORY_SPACE;

	commandStatus |= PCI_INT_DISABLE; // Disable interrupts

	// Write back to the PCI config space
	PciConfigWriteDword(device->bus, device->slot, device->func, 0x04, commandStatus);

	uint32_t bar5 = general.bar[5] & 0xFFFFFFF0;

	// Map BAR5 register as uncacheable
	vmm_MapPage((void*)(uint64_t)bar5, (void*)(uint64_t)bar5, PF_PCD);
	HBA_MEM* hba = (HBA_MEM*)(uint64_t)bar5;

	ahciDevice* ahci = (ahciDevice*)malloc(sizeof(ahciDevice));
	memset(ahci, 0, sizeof(ahciDevice));

	ahci->hba = hba;

	// Section 10.6.3
	// Performs BIOS/OS handoff
	if (hba->cap2 & AHCI_BIOS_HANDOFF)
	{
		dbg_printf("[AHCI] BIOS handoff required!\n");
		// Set OS ownership bit to 1
		hba->bohc |= AHCI_OOS;

		// Spin on the BIOS ownership bit, waiting to be set to 0
		while (hba->bohc & AHCI_BOS);

		// If BIOS busy bit is set to 1 within 25 seconds, the OS shall sleep for 2 seconds
		if (hba->bohc & AHCI_BB)
		{
			sleep(2000);
		}
	}

	// TODO: Reset controller (HBA reset)
	// TOFIX: After the HBA reset, the port signature became invalid!
	// hba->ghc |= AHCI_HR;
	// while (hba->ghc & AHCI_HR);

	// Enable AHCI mode
	hba->ghc |= AHCI_AE;

	AHCI_ProbePort(ahci);
}

