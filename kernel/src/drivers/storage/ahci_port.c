#include "ahci.h"
#include <qemu/print.h>
#include <vmm.h>
#include <utils/memory.h>
#include <x86_64/timer.h>
#include <storage/block.h>
#include <malloc.h>

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier

static char* blockNames[] = {
	"sda",
	"sdb",
	"sdc",
	"sdd",
	"sde",
	"sdf"
};
static int nameOffset = 0;

bool AHCI_PortReady(HBA_PORT* port)
{
	// Spin for maximum 1 second
	uint64_t start = ticks;
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)))
	{
		if (ticks >= start + 1000)
		{
			dbg_printf("[AHCI] Port is hung!\n");
			return false;
		}
	}

	return true;
}

static uint8_t checkType(HBA_PORT* port)
{
	uint32_t ssts = port->ssts;

	uint8_t ipm = (ssts >> 8) & 0x0F; // Interface power management (11:08)
	uint8_t det = ssts & 0x0F; // Device detection (03:00)

	if (det != HBA_PORT_DET_PRESENT)
	{
		return AHCI_DEV_NULL;
	}
	if (ipm != HBA_PORT_IPM_ACTIVE)
	{
		return AHCI_DEV_NULL;
	}

	// Check port's signature
	switch (port->sig)
	{
		case SATA_SIG_ATA:
			return AHCI_DEV_SATA;
		case SATA_SIG_ATAPI:
			return AHCI_DEV_SATAPI;
		case SATA_SIG_SEMB:
			return AHCI_DEV_SEMB;
		case SATA_SIG_PM:
			return AHCI_DEV_PM;
		default:
			return AHCI_DEV_NULL; // Invalid signature
	}
}

void AHCI_ProbePort(ahciDevice* ahci)
{
	uint8_t buffer[512];
	uint32_t pi = ahci->hba->pi;
	for (int i = 0; i < 32; i++)
	{
		if (pi & 1)
		{
			ahciDrive* newDrive = (ahciDrive*)malloc(sizeof(ahciDrive));
			memset(newDrive, 0, sizeof(ahciDrive));
			newDrive->portNum = i;
			newDrive->port = &ahci->hba->ports[i];
			ahci->drives[i] = newDrive;
			AHCI_PortRebase(newDrive);

			int deviceType = checkType(&ahci->hba->ports[i]);
			switch (deviceType)
			{
				case AHCI_DEV_SATA:
					dbg_printf("[AHCI] SATA drive found at port %d\n", i);

					// Send identify ATA
					if (!AHCI_IdentifyATA(newDrive, buffer))
						dbg_printf("Failed to identify ATA at port %d!\n", i);

					// Create a block device
					blockDevice* newBlock = (blockDevice*)malloc(sizeof(blockDevice));
					newBlock->read = AHCI_ReadBlock;

					// Copy model name (Convert to big endian)
					int modelIndex = 0;
					for (int i = 27 * 2; i < 46 * 2; i += 2)
					{
						newBlock->modelName[modelIndex++] = buffer[i + 1];
						newBlock->modelName[modelIndex++] = buffer[i];
					}
					// newBlock->sectorCount = ; // TODO
					newBlock->driverPtr = newDrive;
					newBlock->name = blockNames[nameOffset++];
					newBlock->type = DEV_TYPE_DISK;
					blockRegister(newBlock);
					break;
				case AHCI_DEV_SATAPI:
					dbg_printf("[AHCI] SATAPI drive found at port %d (unsupported)\n", i);
					break;
				case AHCI_DEV_SEMB:
					dbg_printf("[AHCI] SEMB drive found at port %d\n (unsupported)", i);
					break;
				case AHCI_DEV_PM:
					dbg_printf("[AHCI] PM drive found at port %d\n (unsupported)", i);
					break;
				default:
					// AHCI_DEV_NULL
					dbg_printf("[AHCI] No drive found at port %d\n", i);
					break;
			}
		}

		pi >>= 1;
	}
}

void AHCI_PortRebase(ahciDrive* drive)
{
	HBA_PORT* port = drive->port;
	AHCI_StopCmd(port);

	// TODO: Utilize memory more efficently
	
	// Setup command list base address
	void* clbVirt = vmm_Allocate(1);
	void* clbPhys = vmm_VirtToPhys(clbVirt);
	memset(clbVirt, 0, 1024);

	port->clb = (uint32_t)(uint64_t)clbPhys;
	port->clbu = (uint32_t)((uint64_t)clbPhys >> 32);
	drive->clbVirt = clbVirt;

	// Setup FIS
	void* fisVirt = vmm_Allocate(1);
	void* fisPhys = vmm_VirtToPhys(fisVirt);
	memset(fisVirt, 0, 256);

	port->fb = (uint32_t)(uint64_t)fisPhys;
	port->fbu = (uint32_t)((uint64_t)fisPhys >> 32);

	HBA_CMD_HEADER* cmdHeader = (HBA_CMD_HEADER*)clbVirt;

	for (int i = 0; i < 32; i++)
	{
		cmdHeader[i].prdtl = 8; // 256 bytes per command table

		// Wasted a lot of memory lol
		void* cmdTableVirt = vmm_Allocate(1);
		void* cmdTablePhys = vmm_VirtToPhys(cmdTableVirt);
		memset(cmdTableVirt, 0, 256); // 8 * 32 = 256

		uint64_t base = (uint64_t)cmdTablePhys;

		cmdHeader[i].ctba = (uint32_t)base;
		cmdHeader[i].ctbau = (uint32_t)(base >> 32);
		
		drive->ctbaVirt[i] = cmdTableVirt;
	}

	AHCI_StartCmd(port);

	// Clear error
	port->serr = 0xFFFFFFFF;
}

