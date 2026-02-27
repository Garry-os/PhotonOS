#pragma once
#include <stdbool.h>
#include <storage/block.h>
#include <stdint.h>

#define SECTOR_SIZE 512

// End of clusters chain
#define FAT32_EOC (0x0FFFFFF8)
#define FAT32_BAD_CLUSTER (0x0FFFFFF7)

typedef struct
{
	uint8_t name[11]; // 8.3 file name
	uint8_t attributes;
	uint8_t _reserved; // Used by windows NT
	uint8_t creationTimeTenths;
	uint16_t creationTime;
	uint16_t creationDate;
	uint16_t lastAccessedDate;
	uint16_t firstClusterHigh;
	uint16_t lastModificationTime;
	uint16_t lastModificationDate;
	uint16_t firstClusterLow;
	uint32_t size; // File size in bytes
} __attribute__((packed)) fat32DirEntry;

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
	uint32_t firstCluster;
	uint32_t currentCluster;
	uint32_t currentOffset;
	uint32_t currentSector; // Current sector in a cluster
	uint32_t size;

	uint8_t attributes;
	uint8_t buffer[SECTOR_SIZE]; // Buffer for reading sector
} fat32Handle;

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
	fat32Handle* root; // root is a special file

	blockDevice* dev;
} fat32_data;


typedef enum
{
    FAT_ATTRIBUTE_READ_ONLY         = 0x01,
    FAT_ATTRIBUTE_HIDDEN            = 0x02,
    FAT_ATTRIBUTE_SYSTEM            = 0x04,
    FAT_ATTRIBUTE_VOLUME_ID         = 0x08,
    FAT_ATTRIBUTE_DIRECTORY         = 0x10,
    FAT_ATTRIBUTE_ARCHIVE           = 0x20,
    FAT_ATTRIBUTE_LFN               = FAT_ATTRIBUTE_READ_ONLY | FAT_ATTRIBUTE_HIDDEN | FAT_ATTRIBUTE_SYSTEM | FAT_ATTRIBUTE_VOLUME_ID
};

// FAT file handle attributes
#define FAT_HANDLE_DIR (1 << 0)
#define FAT_HANDLE_ROOT (1 << 1)

extern fat32_data* g_data;

bool fat32_mount(blockDevice* dev);

fat32Handle* fat32_open(const char* path);
void fat32_close(fat32Handle* handle);

uint32_t fat32_read(fat32Handle* handle, uint32_t limit, void* buffer);
bool fat32_readEntry(fat32Handle* handle, fat32DirEntry* entry);

// fat32_path.c
bool fat32_traverse(const char* path, fat32DirEntry* out);

// fat32_util.c
uint32_t fat32_clusterToLba(uint32_t cluster);
uint32_t fat32_nextCluster(uint32_t cluster);

void fat32_NameToShort(const char* name, char shortName[12]);

