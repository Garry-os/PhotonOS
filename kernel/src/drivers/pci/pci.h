#pragma once
#include <stdint.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

#define PCI_MAX_BUSES 256
#define PCI_MAX_SLOTS 32
#define PCI_MAX_FUNCTIONS 8

// Header type 0x00 (General)
typedef struct
{
	uint16_t vendorId;
	uint16_t deviceId;
	uint16_t command;
	uint16_t status;
	uint8_t revisionId;
	uint8_t ProgIF; // Program interface
	uint8_t subClassId;
	uint8_t classID;
	uint8_t CacheLineSize;
	uint8_t latencyTimer;
	uint8_t headerType;
	uint8_t BIST;

	// Base addresses
	uint32_t BAR[6];
	
	uint32_t CISPtr;
	uint16_t subSysVendorId;
	uint16_t sysVendorId;
	uint32_t expansionROM;
	uint8_t capabilitiesPtr;

	// Reserved (24 bits)
	
	// Reserved (32 bits)
	
	uint8_t intLine;
	uint8_t intPin;
	uint8_t minGrant;
	uint8_t maxLatency;
} PCIHeader;

uint16_t PciConfigReadWord(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void InitPCI();

