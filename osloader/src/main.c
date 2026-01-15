#include <efi.h>
#include "elf.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

EFI_HANDLE g_ImageHandle;
EFI_SYSTEM_TABLE* g_SystemTable;

// Since we don't have the GNU EFI lib
EFI_GUID EfiLoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
EFI_GUID EfiSimpleFileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_GUID gEfiFileInfoGuid = EFI_FILE_INFO_ID;

void SetupGlobalVars(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	g_ImageHandle = ImageHandle;
	g_SystemTable = SystemTable;
}

void print(CHAR16* str)
{
	g_SystemTable->ConOut->OutputString(g_SystemTable->ConOut, str);
}

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

int memcmp(const void* a, const void* b, size_t num)
{
	const uint8_t* u8A = (uint8_t*)a;
	const uint8_t* u8B = (uint8_t*)b;

	for (size_t i = 0; i < num; i++)
	{
		if (u8A[i] != u8B[i])
		{
			return u8A[i] - u8B[i];
		}
	}

	return 0;
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

bool VerifyElfHeader(Elf64_Ehdr header)
{
	if (memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT)
	{
		return false;
	}

	return true;
}

typedef void (*KernelEntry)();

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	// Setup global vars
	SetupGlobalVars(ImageHandle, SystemTable);
	
	// Clear screen
	SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

	print(L"Osloader v0.0.1\r\n");
	print(L"Press any key to boot...\r\n");

	// Wait for key
	EFI_INPUT_KEY key;
	while (SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key) != EFI_SUCCESS);

	print(L"Booting...\r\n");

	// Load the kernel file
	EFI_FILE* KernelFile = LoadFile(NULL, L"\\kernel.bin");
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

	print(L"Kernel loaded...\r\n");

	KernelEntry entry = (KernelEntry)header.e_entry;
	entry();

	while (1);

	return EFI_SUCCESS; // Completed successfully
}


