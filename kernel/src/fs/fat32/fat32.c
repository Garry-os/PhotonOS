//
// FAT32 read-only (RO) driver
//
#include "fat32.h"
#include <stdint.h>
#include <malloc.h>
#include <console.h>
#include <qemu/print.h>

#define SECTOR_SIZE 512

// FAT EBR (Include FAT12/FAT16 EBR with FAT32 EBR)
typedef struct
{
	uint32_t sectorsPerFAT;
	uint16_t flags;
	uint16_t version; // High byte is major version & low is minor version
	uint32_t rootDirCluster; // Often 2
	uint16_t fsInfoSector;
	uint16_t backupBS_Sector;
	uint8_t _reserved0[12];

	uint8_t driveNum;
	uint8_t _reserved1; // Flags in windows NT, reserved otherwise
	uint8_t signature; // 0x28 or 0x29
	uint32_t volumeID; // Serial number
	uint8_t volumeLabel[11];
	uint8_t SystemID;
} __attribute__((packed)) fat_ebr;

// FAT32 boot sector
typedef struct
{
	// FAT32 BPB (BIOS parameter block)
	uint8_t jumpIns[3]; // JMP SHORT 3C NOP
	uint8_t OEMId[8];
	uint16_t bytesPerSector;
	uint8_t sectorsPerCluster;
	uint16_t reservedSectors;
	uint8_t fatCount; // File allocations count
	uint16_t rootDirEntries;
	uint16_t totalSectors; // If value is 0, it means total > 65535 sectors
	uint8_t mediaDescType;
	uint16_t sectorsPerFAT; // FAT12/FAT16 only
	uint16_t sectorsPerTrack;
	uint16_t headsCount;
	uint32_t hiddenSectors;
	uint32_t largeSectors;

	fat_ebr ebr;

	// The rest is the boot code...
} __attribute__((packed)) fat32_bs;

typedef struct
{
	union
	{
		fat32_bs data;
		uint8_t bytes[SECTOR_SIZE];
	} bootSector;

	uint8_t minorVersion;
	uint8_t majorVersion;

	uint32_t fatStartLba;
	uint32_t clusterStartLba;
} fat32_data;

static fat32_data* g_data;

// Mount a partition block device
bool fat32_mount(blockDevice* dev)
{
	// Allocate memory
	g_data = (fat32_data*)malloc(sizeof(fat32_data));

	// Read the boot sector
	if (!dev->read(dev, 0, 1, g_data->bootSector.bytes))
	{
		goto error;
	}

	// Check for FAT signature (0x28 or 0x29)
	bool valid = g_data->bootSector.data.ebr.signature == 0x28 || g_data->bootSector.data.ebr.signature == 0x29;
	if (!valid)
	{
		dbg_printf("[FAT32] Invalid FAT32 signature on dev %s, signature: 0x%x\n", dev->name, g_data->bootSector.data.ebr.signature);
		goto error;
	}

	// Get FAT32 version
	g_data->minorVersion = g_data->bootSector.data.ebr.version & 0xFF;
	g_data->majorVersion = (g_data->bootSector.data.ebr.version >> 8) & 0xFF;

	// Calculate where FAT and cluster start
	g_data->fatStartLba = dev->lbaStart + g_data->bootSector.data.reservedSectors;
	g_data->clusterStartLba = g_data->fatStartLba + (g_data->bootSector.data.ebr.sectorsPerFAT * g_data->bootSector.data.fatCount);

	printf("Mounted dev %s, FAT version %d.%d\n", dev->name, g_data->majorVersion, g_data->minorVersion);
	return true;
error:
	printf("Failed to mount device %s\n", dev->name);
	return false;
}

