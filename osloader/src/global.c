#include "global.h"

EFI_SYSTEM_TABLE* g_SystemTable;
EFI_HANDLE g_ImageHandle;

void SetupGlobalVars(EFI_SYSTEM_TABLE* SystemTable, EFI_HANDLE ImageHandle)
{
	g_SystemTable = SystemTable;
	g_ImageHandle = ImageHandle;
}

