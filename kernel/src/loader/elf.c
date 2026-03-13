//
// 64-bit ELF loader
//
#include "elf.h"
#include <stdbool.h>
#include <vfs/vfs.h>
#include <malloc.h>
#include <vmm.h>
#include <paging.h>
#include <pmm.h>
#include <qemu/print.h>
#include <utils/memory.h>

// Verify if the file is compatible?
bool elfVerify(Elf64_Ehdr* header)
{
	if (!header)
	{
		return false;
	}

	// Check magic
	if (header->e_ident[EI_MAG0] != ELFMAG0 ||
		header->e_ident[EI_MAG1] != ELFMAG1 ||
		header->e_ident[EI_MAG2] != ELFMAG2 ||
		header->e_ident[EI_MAG3] != ELFMAG3)
	{
		dbg_printf("[ELF] Incorrect header magic.\n");
		return false;
	}

	if (header->e_ident[EI_CLASS] != ELFCLASS64 || 
		header->e_machine != ELF_x86_64_MACHINE)
	{
		dbg_printf("[ELF] Architecture isn't supported!\n");
		return false;
	}

	return true;
}

task_t* elfLoad(char* filePath)
{
	fileHandle* file = fsOpen(filePath);
	if (!file)
	{
		return NULL;
	}

	task_t* task = NULL;

	// Read the whole file
	uint8_t* buffer = (uint8_t*)malloc(sizeof(Elf64_Ehdr));
	if (fsRead(file, sizeof(Elf64_Ehdr), buffer) != sizeof(Elf64_Ehdr))
	{
		dbg_printf("[ELF] Failed to read ehdr\n");
		goto cleanup;
	}

	// Verify the file
	Elf64_Ehdr* ehdr = (Elf64_Ehdr*)buffer;
	if (!elfVerify(ehdr))
	{
		goto cleanup;
	}

	uint64_t* oldPd = vmm_GetCurrentPd();
	uint64_t* elfPd = vmm_CopyKernelPd();
	vmm_SwitchPd(elfPd);

	// Read in the phdrs into a buffer
	fsSeek(file, ehdr->e_phoff, SEEK_SET);
	size_t phdrsSize = ehdr->e_phnum * ehdr->e_phentsize;
	Elf64_Phdr* phdrs = (Elf64_Phdr*)malloc(phdrsSize);
	// Read in PHDRs
	fsRead(file, phdrsSize, (uint8_t*)phdrs);

	// Loop through each phdr
	for (int i = 0; i < ehdr->e_phnum; i++)
	{
		Elf64_Phdr* phdr = (Elf64_Phdr*)((size_t)phdrs + ehdr->e_phentsize * i);

		// Only load loadable segments
		switch (phdr->p_type)
		{
			case PT_LOAD:
				// Allocate virtual memory
				size_t pages = (phdr->p_memsz + PAGE_SIZE - 1) / PAGE_SIZE; // Round up
				for (size_t j = 0; j < pages; j++)
					vmm_MapPage((void*)(phdr->p_vaddr + j), pmm_Allocate(1), PF_USER);

				// Copy segment data
				fsSeek(file, phdr->p_offset, SEEK_SET);
				size_t size = phdr->p_filesz;
				fsRead(file, size, (void*)phdr->p_vaddr);
				
				// Zeros out the BSS
				if (phdr->p_memsz > phdr->p_filesz)
				{
					memset((void*)phdr->p_vaddr + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
				}

				break;
		}
	}

	free(phdrs);

	// Done loading ELF segments
	vmm_SwitchPd(oldPd);

	uint64_t entry = ehdr->e_entry;
	task = TaskCreate(entry, elfPd, false);
cleanup:
	fsClose(file);
	free(buffer);
	return task;
}

