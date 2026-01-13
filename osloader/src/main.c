#include <efi.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Press any key...\r\n");

	EFI_INPUT_KEY key;
	while (SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key) != EFI_SUCCESS)
	{

	}

	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Key Pressed\r\n");

	while (1);

	return EFI_SUCCESS; // Completed successfully
}


