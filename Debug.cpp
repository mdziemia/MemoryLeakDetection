// -----------------------------------------------------------------------------
// Simple Memory leak detection 
// 2008-11-03
//
// Maciej Dziemianczuk
// inf.ug.edu.pl/~mdziemia/
//
// Based on Wu Yongwei's debug_new.cpp
// http://sourceforge.net/projects/nvwa
// for WIN32 with gcc compiler
// -----------------------------------------------------------------------------

#ifndef _DEBUG_NO
#include <stdio.h>
#include <string.h>

#define _FILE_CPP_DEBUG
#include "Debug.h"

// Static members
unsigned int _exit_detect::count = 0;
unsigned long __memory_allocated = 0;


// First element of the memories list
static struct SMemList _LeakRoot = 
{
	&_LeakRoot, &_LeakRoot,
	0, false,
	NULL, 0,
	__Tag
};


// -----------------------------------------------------------------------------
// Allocate memory with SMemList block at the begining
void* AllocateMem(size_t _size, bool _array, char *_file, unsigned _line)
{
	// calculate new size
	size_t newSize = sizeof(struct SMemList) + _size;
	
	//printf("Allocate %d %d file %s %d\n", _size, _array, _file, _line);

	// create new element of the list
	struct SMemList *newElem = (struct SMemList*)malloc(newSize);
	
	newElem->next = _LeakRoot.next;
	newElem->prev = &_LeakRoot;
	newElem->size = _size;
	newElem->array = _array;
	newElem->file = NULL;
	
	if (_file)
	{
		newElem->file = (char *)malloc(strlen(_file)+1);
		strcpy(newElem->file, _file);
	}
	newElem->line = _line;
	newElem->tag = __Tag;
		
	// update list
	_LeakRoot.next->prev = newElem;
	_LeakRoot.next = newElem;
	
	__memory_allocated += _size;
	
	return (char*)newElem + sizeof(struct SMemList);
}

// -----------------------------------------------------------------------------
// Free memory with block, update list of leaks
void  DeleteMem(void* _ptr, bool _array)
{
	// Go to begin of SMemList block
	struct SMemList *currentElem = (struct SMemList *)((char *)_ptr - sizeof(struct SMemList));
	
	// Check for Tag
	if (currentElem->tag != __Tag)
	{
		printf("Invalid delete%s at %p %d\n", (_array)?"[]":"", _ptr, currentElem->size);
		return;
	}
	
	if (currentElem->array != _array)
	{
		printf("Delete%s before or after mem%s at %p (new%s file %s line %d)\n",  _array?"[]":"", _array?"":"[]", _ptr, currentElem->array?"[]":"", currentElem->file, currentElem->line);
		return;
	}
	
	// update leaks' list
	currentElem->tag = 0;
	currentElem->prev->next = currentElem->next;
	currentElem->next->prev = currentElem->prev;
	__memory_allocated -= currentElem->size;
	
	if (currentElem->file) free (currentElem->file);
	free(currentElem);
}

// -----------------------------------------------------------------------------
// Check for leaks
unsigned int MemoryLeak(void)
{
	unsigned int count = 0;
	struct SMemList *currentElem;
	
	// Go through all the list
	SMemList *ptr = _LeakRoot.next;
	while (ptr && ptr != &_LeakRoot)
	{
		if (ptr->tag != __Tag)
		{
			printf("Memory: heap data corrupt near %p %d\n", ptr, (char*)currentElem->size + sizeof(struct SMemList));
		}
		
		// Memory leak
		printf("Leak%s %p size %d", ptr->array?"[]":"  ", ptr, ptr->size);
		
		if (ptr->file)
			printf(" (new%s file %s line %d)", ptr->array?"[]":"", ptr->file, ptr->line);
			
		printf("\n");
		
		++ count;
		ptr = ptr->next;
	}
	
	if (count)
	{
		printf("\nThere is %d leaks with %d bytes memory left!\n", count, __memory_allocated);
	}
	return count;
}

// -----------------------------------------------------------------------------
// Operator new and delete overloading
// -----------------------------------------------------------------------------
void* operator new(size_t _size) 
{
	return AllocateMem(_size, false, NULL, 0);
}
void* operator new[](size_t _size)
{
	return AllocateMem(_size, true, NULL, 0);
}
// -----------------------------------------------------------------------------
void* operator new(size_t _size, char *_file, unsigned int _line)
{
	return AllocateMem(_size, false, _file, _line);
}
void* operator new[](size_t _size, char *_file, unsigned int _line)
{
	return AllocateMem(_size, true, _file, _line);
}

// -----------------------------------------------------------------------------
void operator delete(void *_ptr)
{
	DeleteMem(_ptr, false);
}
void operator delete[](void *_ptr)
{
	DeleteMem(_ptr, true);
}

#endif
