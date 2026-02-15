//
// Legacy PCI configuration via CPU 32-bit I/O
//
#include "pci.h"
#include <x86_64/cpu.h>
#include <console.h>
#include <utils/memory.h>
#include <malloc.h>

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
    tmp = (uint16_t)((x86_inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
	return tmp;
}

uint32_t PciConfigReadDword(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	uint32_t address;
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint32_t value = 0;

	// Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
	
	// Write out the address
	x86_outl(PCI_CONFIG_ADDRESS, address);

	// Read in the data
	value = x86_inl(PCI_CONFIG_DATA);
	return value;
}

// Check if a device is valid or not
int PciCheckDevice(uint16_t bus, uint8_t slot, uint8_t func)
{
	uint16_t vendorId = PciConfigReadWord(bus, slot, func, 0x00);
	return !(vendorId == 0xFFFF || !vendorId);
}

// Just essentially read the config address & copy it into the struct
void PciGetHeader(PCIHeader* header, uint16_t bus, uint8_t slot, uint8_t func)
{
	uint32_t* headerPtr = (uint32_t*)header;
	for (uint32_t i = 0; i < sizeof(PCIHeader); i += 4)
	{
		headerPtr[i / 4] = PciConfigReadDword(bus, slot, func, i);
	}
}

void PciGetGeneralHeader(PCIGeneralHeader* header, uint16_t bus, uint8_t slot, uint8_t func)
{
	uint32_t* headerPtr = (uint32_t*)header;
	for (uint32_t i = 0x10; i < sizeof(PCIGeneralHeader); i++) // 0x10 is where the BAR start
	{
		headerPtr[i / 4] = PciConfigReadDword(bus, slot, func, i);
	}
}



void InitPCI()
{
	PCIHeader* header = (PCIHeader*)malloc(sizeof(PCIHeader));

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

				PciGetHeader(header, bus, slot, func);

				// Clear bit 7
				if ((header->headerType & ~(1 << 7)) != PCI_GENERAL)
				{
					continue;
				}

				// Print out the devices' names
				printf("PCI device found, vendor ID: %x, device ID: %x\n", header->vendorId, header->deviceId);
			}
		}
	}

	free(header);
}

