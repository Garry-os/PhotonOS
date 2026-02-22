#include "ahci.h"
#include <vmm.h>
#include <utils/memory.h>
#include <qemu/print.h>

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000

int AHCI_FindCmdSlot(HBA_PORT* port)
{
	// If not set in SACT and CI, slot is free
	uint32_t slots = (port->sact | port->ci);

	for (int i = 0; i < 32; i++)
	{
		if ((slots & 1) == 0)
		{
			return i;
		}

		slots >>= 1;
	}

	dbg_printf("[AHCI] Can't find free cmd slot!\n");
	return -1;
}

HBA_CMD_TBL* AHCI_SetupCmd(ahciDevice* ahci, int portNum, int slot, uint32_t count, void* buffer, bool write)
{
	// Setup command header
	HBA_CMD_HEADER* cmdHeader = (HBA_CMD_HEADER*)ahci->clbVirt[portNum];
	cmdHeader = &cmdHeader[slot];
	memset(cmdHeader, 0, sizeof(HBA_CMD_HEADER));

	cmdHeader->cfl = sizeof(FIS_TYPE_REG_H2D) / sizeof(uint32_t); // Command FIS size
	cmdHeader->w = (uint8_t)write;
	cmdHeader->prdtl = 1; // TODO

	void* ctbaVirt = ahci->ctbaVirt[portNum][slot];
	void* ctbaPhys = vmm_VirtToPhys(ctbaVirt);
	cmdHeader->ctba = (uint32_t)(uint64_t)ctbaPhys;
	cmdHeader->ctbau = (uint32_t)((uint64_t)ctbaPhys >> 32);

	HBA_CMD_TBL* cmdTbl = (HBA_CMD_TBL*)((uint64_t)ahci->ctbaVirt[portNum][slot]);
	memset(cmdTbl, 0, sizeof(HBA_CMD_TBL) + cmdHeader->prdtl * sizeof(HBA_PRDT_ENTRY));

	// Setup buffer
	void* bufferPhys = vmm_VirtToPhys(buffer);
	cmdTbl->prdt_entry[0].dba = (uint32_t)(uint64_t)bufferPhys;
	cmdTbl->prdt_entry[0].dbau = (uint32_t)((uint64_t)bufferPhys >> 32);
	cmdTbl->prdt_entry[0].dbc = (count << 9) - 1;
	cmdTbl->prdt_entry[0].i = 1;

	return cmdTbl;
}

// Setup a H2D FIS
void AHCI_SetupFIS(HBA_CMD_TBL* cmdTbl, uint8_t command, uint64_t lba, uint32_t count)
{
	FIS_REG_H2D* cmdFis = (FIS_REG_H2D*)&cmdTbl->cfis;

	cmdFis->fis_type = FIS_TYPE_REG_H2D;
	cmdFis->c = 1;
	cmdFis->command = command;

	// Setup LBA
	uint32_t lbaLow = (uint32_t)lba;
	uint32_t lbaHigh = (uint32_t)(lba >> 32);

	// Only support up to 48 bits LBA
	cmdFis->lba0 = (uint8_t)lbaLow;
	cmdFis->lba1 = (uint8_t)(lbaLow >> 8);
	cmdFis->lba2 = (uint8_t)(lbaLow >> 16);
	cmdFis->lba3 = (uint8_t)(lbaLow >> 24);
	
	cmdFis->lba4 = (uint8_t)lbaHigh;
	cmdFis->lba5 = (uint8_t)(lbaHigh >> 8);

	cmdFis->device = 1 << 6; // LBA mode
	
	// Setup sector count (16 bit)
	cmdFis->countl = count & 0xFF;
	cmdFis->counth = (count >> 8) & 0xFF;
}

bool AHCI_IssueCmd(HBA_PORT* port, int slot)
{
	port->ci = 1 << slot;

	// Spin
	while (1)
	{
		if ((port->ci & (1 << slot)) == 0)
			break;
		if (port->is & HBA_PxIS_TFES)
		{
			// Task file error
			dbg_printf("[AHCI] Task file error!\n");
			return false;
		}
	}

	if (port->is & HBA_PxIS_TFES)
	{
		// Task file error
		dbg_printf("[AHCI] Task file error!\n");
		return false;
	}

	return true;
}

bool AHCI_Read(ahciDevice* ahci, int portNum, uint64_t lba, uint32_t count, void* buffer)
{
	HBA_PORT* port = &ahci->hba->ports[portNum];
	port->is = (uint32_t)-1; // Clear pending interrupts
	
	// Find a free command slot
	int slot = AHCI_FindCmdSlot(port);
	if (slot == -1)
		return false;

	// Setup command
	HBA_CMD_TBL* cmdTbl = AHCI_SetupCmd(ahci, portNum, slot, count, buffer, false);

	// Setup FIS
	AHCI_SetupFIS(cmdTbl, ATA_CMD_READ_DMA_EX, lba, count);

	if (!AHCI_PortReady(port))
		return false;
	
	// Issue command
	bool success = AHCI_IssueCmd(port, slot);
	return success;
}

void AHCI_StartCmd(HBA_PORT* port)
{
	// Wait until CR is cleared
	while (port->cmd & HBA_PxCMD_CR);

	// Set FRE and ST
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}

void AHCI_StopCmd(HBA_PORT* port)
{
	// Clear ST
	port->cmd &= ~(HBA_PxCMD_ST);

	// Clear FRE
	port->cmd &= ~(HBA_PxCMD_FRE);

	// Wait until FR and CR are cleared
	while (1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;

		break;
	}
}

