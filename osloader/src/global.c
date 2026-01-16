#include "global.h"

EFI_SYSTEM_TABLE* g_SystemTable;
EFI_HANDLE g_ImageHandle;

// Since we don't have the GNU EFI lib
EFI_GUID EfiLoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
EFI_GUID EfiSimpleFileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_GUID gEfiFileInfoGuid = EFI_FILE_INFO_ID;

void SetupGlobalVars(EFI_SYSTEM_TABLE* SystemTable, EFI_HANDLE ImageHandle)
{
	g_SystemTable = SystemTable;
	g_ImageHandle = ImageHandle;
}

