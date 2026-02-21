#include "ahci.h"
#include <qemu/print.h>

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier

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
	uint32_t pi = ahci->hba->pi;
	for (int i = 0; i < 32; i++)
	{
		if (pi & 1)
		{
			int deviceType = checkType(&ahci->hba->ports[i]);
			switch (deviceType)
			{
				case AHCI_DEV_SATA:
					dbg_printf("[AHCI] SATA drive found at port %d\n", i);
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

