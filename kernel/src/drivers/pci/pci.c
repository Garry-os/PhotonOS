//
// Legacy PCI configuration via CPU 32-bit I/O
//
#include "pci.h"
#include <x86_64/cpu.h>
#include <console.h>
#include <utils/memory.h>
#include <malloc.h>
#include <storage/ahci.h>
#include <qemu/print.h>
#include <utils/llist.h>

PCIDevice* firstPCIDevice;

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

void PciConfigWriteDword(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data)
{
	uint32_t address;
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;

	// Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
	
	// Write out the address
	x86_outl(PCI_CONFIG_ADDRESS, address);

	// Write out the data
	x86_outl(PCI_CONFIG_DATA, data);
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
	for (uint32_t i = 0x10; i < sizeof(PCIGeneralHeader); i += 4) // 0x10 is where the BAR start
	{
		headerPtr[(i - 0x10) / 4] = PciConfigReadDword(bus, slot, func, i);
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

				// Clear bit 7 to check for general header
				if ((header->headerType & ~(1 << 7)) != PCI_GENERAL)
				{
					continue;
				}

				PCIDevice* device = (PCIDevice*)LL_Allocate((void**)&firstPCIDevice, sizeof(PCIDevice));
				device->bus = bus;
				device->slot = slot;
				device->func = func;

				device->vendorId = header->vendorId;
				device->deviceId = header->deviceId;

				device->header = *header;
				PciGetGeneralHeader(&device->generalHeader, bus, slot, func);

				// Initialize PCI devices' drivers
				switch (header->classId)
				{
					case PCI_MASS_STORAGE_CONTROLLER:
						if (header->subClassId == PCI_SATA)
						{
							if (header->progIF == PCI_AHCI_1_0)
							{
								printf("Detected AHCI 1.0 SATA controller.\n");
								InitAHCI(device);
							}
						}
						break;
				}
			}
		}
	}

	free(header);
}

