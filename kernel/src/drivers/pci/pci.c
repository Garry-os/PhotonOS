//
// Legacy PCI configuration via CPU 32-bit I/O
//
#include "pci.h"
#include <x86_64/cpu.h>
#include <console.h>

uint16_t PciConfigReadWord(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	uint32_t address;
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint16_t tmp = 0;

	// Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
	
	// Write out the address
	x86_outl(PCI_CONFIG_ADDRESS, address);

	// Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((x86_inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
	return tmp;
}

// Check if a device is valid or not
int PciCheckDevice(uint16_t bus, uint8_t slot, uint8_t func)
{
	uint16_t vendorId = PciConfigReadWord(bus, slot, func, 0x00);
	return !(vendorId == 0xFFFF || !vendorId);
}

void InitPCI()
{
	// Enumerate through the bus
	for (uint16_t bus = 0; bus < PCI_MAX_BUSES; bus++)
	{
		for (uint8_t slot = 0; slot < PCI_MAX_SLOTS; slot++)
		{
			for (uint8_t func = 0; func < PCI_MAX_FUNCTIONS; func++)
			{
				if (!PciCheckDevice(bus, slot, func))
				{
					// Non-existance device
					continue;
				}

				// Print out the devices' names
				printf("PCI device found, vendor ID: %x, device ID: %x\n", PciConfigReadWord(bus, slot, func, 0x00), 
						PciConfigReadWord(bus, slot, func, 0x02));
			}
		}
	}
}

