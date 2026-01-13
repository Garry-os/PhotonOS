#include <efi.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello World!\r\n");

	while (1);

	return EFI_SUCCESS; // Completed successfully
}


