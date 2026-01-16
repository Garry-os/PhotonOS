#include <efi.h>
#include "gop.h"
#include "global.h"

Fb_Info fb_info;

Fb_Info* GetGOPInfo()
{
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS status;

	// Locate GOP protocol
	status = g_SystemTable->BootServices->LocateProtocol(
		&gopGuid,
		NULL,
		(void**)&gop
	);

	if (EFI_ERROR(status))
	{
		return NULL;
	}

	// Fill out the information size
	fb_info.base = (void*)gop->Mode->FrameBufferBase;
	fb_info.fbSize = gop->Mode->FrameBufferSize;
	fb_info.width = gop->Mode->Info->HorizontalResolution;
	fb_info.height = gop->Mode->Info->VerticalResolution;
	fb_info.pixelsPerScanline = gop->Mode->Info->PixelsPerScanLine;

	return &fb_info;
}

