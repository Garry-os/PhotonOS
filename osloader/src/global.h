#pragma once
#include <efi.h>

extern EFI_SYSTEM_TABLE* g_SystemTable;
extern EFI_HANDLE g_ImageHandle;

extern EFI_GUID EfiLoadedImageProtocolGuid;
extern EFI_GUID EfiSimpleFileSystemProtocolGuid;
extern EFI_GUID gEfiFileInfoGuid;

void SetupGlobalVars(EFI_SYSTEM_TABLE* SystemTable, EFI_HANDLE ImageHandle);

