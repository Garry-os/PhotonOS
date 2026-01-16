#include <efi.h>
#include "elf.h"
#include "file.h"
#include "global.h"
#include "gop.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <osloader.h>

void print(CHAR16* str)
{
	g_SystemTable->ConOut->OutputString(g_SystemTable->ConOut, str);
}

void PressKeyToReboot()
{
	print(L"Press any key to reboot...\r\n");
	EFI_INPUT_KEY key;
	while (g_SystemTable->ConIn->ReadKeyStroke(g_SystemTable->ConIn, &key) != EFI_SUCCESS);

	// Perform a cold reset (hard reset)
	g_SystemTable->RuntimeServices->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);

	// Should never reach
	__builtin_unreachable();
}

typedef void (*KernelEntry)(BootInfo info);

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	// Setup global vars
	SetupGlobalVars(SystemTable, ImageHandle);
	
	// Clear screen
	SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

	print(L"Osloader v0.0.1\r\n");
	print(L"Press any key to boot...\r\n");

	// Wait for key
	EFI_INPUT_KEY key;
	while (SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key) != EFI_SUCCESS);

	print(L"Booting...\r\n");

	// Load the kernel file
	EFI_FILE* KernelFile = LoadFile(NULL, L"\\EFI\\BOOT\\kernel.bin");
	if (KernelFile == NULL)
	{
		print(L"Failed to load kernel file!\r\n");
		PressKeyToReboot();
	}

	Elf64_Ehdr header;
	// Read the ELF Ehdr header
	UINTN size = sizeof(header);
	KernelFile->Read(KernelFile, &size, &header);

	bool result = VerifyElfHeader(header);
	if (!result)
	{
		print(L"Invalid kernel ELF header format!\r\n");
		PressKeyToReboot();
	}

	// Read the program header
	Elf64_Phdr* phdrs;
	KernelFile->SetPosition(KernelFile, header.e_phoff);
	size = header.e_phnum * header.e_phentsize;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
	KernelFile->Read(KernelFile, &size, phdrs);

	// Loop through the program headers
	for (
		Elf64_Phdr* phdr = phdrs;
		(uint8_t*)phdr < (uint8_t*)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((uint8_t*)phdr + header.e_phentsize)
	)
	{
		switch (phdr->p_type)
		{
			case PT_LOAD:
				// Loadable segment
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000; // Round up
				Elf64_Addr seg = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &seg);

				KernelFile->SetPosition(KernelFile, phdr->p_offset);
				size = phdr->p_filesz;
				KernelFile->Read(KernelFile, &size, (void*)seg);
				break;
		}
	}

	// Get GOP info
	Fb_Info* fbInfo = GetGOPInfo();
	if (fbInfo == NULL)
	{
		print(L"Failed to get GOP information\r\n");
		PressKeyToReboot();
	}

	print(L"Kernel loaded...\r\n");

	// Setup boot info
	BootInfo info;
	info.magic = (uint32_t)BOOT_MAGIC;
	info.kernelPhysicalAddress = phdrs->p_paddr;
	info.fbInfo = *fbInfo;

	KernelEntry entry = (KernelEntry)header.e_entry;
	entry(info);

	// Should never reach here

	return EFI_SUCCESS; // Completed successfully
}


