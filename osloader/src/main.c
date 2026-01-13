#include <efi.h>

EFI_HANDLE g_ImageHandle;
EFI_SYSTEM_TABLE* g_SystemTable;

void SetupGlobalVars(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	g_ImageHandle = ImageHandle;
	g_SystemTable = SystemTable;
}

void print(CHAR16* str)
{
	g_SystemTable->ConOut->OutputString(g_SystemTable->ConOut, str);
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	// Setup global vars
	SetupGlobalVars(ImageHandle, SystemTable);

	print(L"Press any key to boot...\r\n");

	EFI_INPUT_KEY key;
	while (SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key) != EFI_SUCCESS)
	{

	}

	print(L"Booting...\r\n");

	while (1);

	return EFI_SUCCESS; // Completed successfully
}


