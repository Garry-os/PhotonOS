#pragma once
#include <stdint.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

#define PCI_MAX_BUSES 256
#define PCI_MAX_SLOTS 32
#define PCI_MAX_FUNCTIONS 8

// PCI header types
#define PCI_GENERAL 0x0
#define PCI_TO_PCI  0x1
#define PCI_TO_CARDBUS 0x2

// PCI header
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
} __attribute__((packed)) PCIHeader;

// PCI general header (header type 0x00)
typedef struct
{
	// Base addresses
	uint32_t BAR[6];
	
	uint32_t CISPtr;
	uint16_t subSysVendorId;
	uint16_t sysVendorId;
	uint32_t expansionROM;
	uint8_t capabilitiesPtr;

	// Reserved (24 bits)
	uint16_t _reserved0;
	uint8_t _reserved1;

	// Reserved (32 bits)
	uint32_t _reserved2;

	uint8_t intLine;
	uint8_t intPin;
	uint8_t minGrant;
	uint8_t maxLatency;
} __attribute__((packed)) PCIGeneralHeader;

uint16_t PciConfigReadWord(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint32_t PciConfigReadDword(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void InitPCI();

