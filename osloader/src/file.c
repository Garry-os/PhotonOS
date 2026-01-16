#include "file.h"
#include "global.h"

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path)
{
	EFI_FILE* File;

	EFI_LOADED_IMAGE_PROTOCOL* ImageProtocol;
	g_SystemTable->BootServices->HandleProtocol(g_ImageHandle, &EfiLoadedImageProtocolGuid, (void**)&ImageProtocol);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
	g_SystemTable->BootServices->HandleProtocol(ImageProtocol->DeviceHandle, &EfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);

	if (Directory == NULL)
	{
		FileSystem->OpenVolume(FileSystem, &Directory);
	}

	EFI_STATUS status = Directory->Open(Directory, &File, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

	if (EFI_ERROR(status))
	{
		return NULL;
	}

	return File;
}

