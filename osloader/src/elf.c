#include "elf.h"
#include <stdbool.h>
#include "memory.h"

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

