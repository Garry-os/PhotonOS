//
// Linked list data structure implementation for the kernel
// 1 -> 2 -> 3 -> ...
//
#include "llist.h"
#include <malloc.h>
#include <utils/memory.h>

// first: The first linked list header in the entire linked list
void* LL_Allocate(void** first, size_t size)
{
	LLHeader* target = (LLHeader*)malloc(size);
	memset(target, 0, size);

	LLHeader* current = (LLHeader*)(*first);
	while (1)
	{
		if (current == NULL)
		{
			// First one
			*first = target;
			break;
		}

		if (current->next == NULL)
		{
			// End of linked list
			current->next = target;
			break;
		}

		current = current->next;
	}

	target->next = NULL;
	return target;
}

// Add an element to the linked list
// Does not allocate like LL_Allocate()
void LL_Add(void** first, void* ptr)
{
	LLHeader* target = (void*)ptr;
	LLHeader* current = (LLHeader*)(*first);
	while (1)
	{
		if (current == NULL)
		{
			// First one
			*first = target;
			break;
		}

		if (current->next == NULL)
		{
			// End of linked list
			current->next = target;
			break;
		}

		current = current->next;
	}

	target->next = NULL;
}

