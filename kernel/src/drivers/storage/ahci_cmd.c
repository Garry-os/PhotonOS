#include "ahci.h"

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000

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

