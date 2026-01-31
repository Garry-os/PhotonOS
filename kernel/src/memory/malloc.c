//
// Malloc (heap) implementation for the kernel
//
#include "malloc.h"
#include <stdbool.h>
#include <vmm.h>
#include <pmm.h>
#include <qemu/print.h>

// Heap initial size
// 16 pages
#define HEAP_PAGES 0x20
#define HEAP_ALIGNMENT 0x10

// Malloc node
typedef struct mNode
{
	size_t size;
	bool free; // Free or not?
	struct mNode* next;
	struct mNode* prev;
} mNode_t;

void* mStart;
void* mEnd;
mNode_t* lastNode;

void InitHeap()
{
	void* start = vmm_Allocate(HEAP_PAGES);
	dbg_printf("[Heap] Start address: 0x%llx\n", start);
	size_t heapSize = HEAP_PAGES * PAGE_SIZE; // Heap size in bytes
	
	mStart = start;
	mEnd = (void*)((uint64_t)start + heapSize);

	// Setup the first node
	lastNode = (mNode_t*)mStart;
	lastNode->size = heapSize - sizeof(mNode_t);
	lastNode->next = NULL;
	lastNode->prev = NULL;
	lastNode->free = true;
}

size_t align(size_t size)
{
	if (size % HEAP_ALIGNMENT > 0)
	{
		size -= size % HEAP_ALIGNMENT;
		size += HEAP_ALIGNMENT;
	}

	return size;
}

mNode_t* splitNode(mNode_t* node, size_t size)
{
	if (size < HEAP_ALIGNMENT)
	{
		// Not large enough
		return NULL;
	}

	// Setup the new node
	mNode_t* newNode = (mNode_t*)((void*)node + size + sizeof(mNode_t));
	newNode->size = node->size - size - sizeof(mNode_t);
	newNode->free = true;
	newNode->next = node->next;

	// Re-setup current node
	node->size = size;
	node->free = false;
	node->next = newNode;

	if (lastNode == node)
	{
		// Update the last node
		lastNode = newNode;
	}

	return newNode;
}

void expandHeap(size_t size)
{
	dbg_printf("[Heap] Expanding heap!\n");
	size = align(size);

	size_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	mNode_t* newNode = (mNode_t*)vmm_Allocate(pages);

	newNode->next = NULL;
	newNode->prev = lastNode;
	newNode->size = pages * PAGE_SIZE;
	newNode->free = true;

	// Update last node
	lastNode->next = newNode;
	lastNode = newNode;
}

void* malloc(size_t size)
{
	if (!size)
	{
		dbg_printf("[Heap] Can't allocate with size 0!\n");
		return NULL;
	}

	mNode_t* current = (mNode_t*)mStart;
	size = align(size);

	while (current != NULL)
	{
		if (current->free)
		{
			if (current->size >= size)
			{
				// A large enough node
				if (current->size > size)
				{
					splitNode(current, size);
					current->free = false;
					current->size = size;
				}
				else
				{
					current->free = false;
				}
				
				return (void*)current + sizeof(mNode_t);
			}
		}

		if (current == lastNode)
		{
			expandHeap(size);
			if (current->prev != NULL)
			{
				current = current->prev;
			}
		}

		current = current->next;
	}

	return NULL;
}

