#pragma once
#include <efi.h>

extern EFI_SYSTEM_TABLE* g_SystemTable;
extern EFI_HANDLE g_ImageHandle;

void SetupGlobalVars(EFI_SYSTEM_TABLE* SystemTable, EFI_HANDLE ImageHandle);

