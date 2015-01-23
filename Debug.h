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
#ifndef _CLASS_DEBUG_NEW
#define _CLASS_DEBUG_NEW

#include <stdio.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------
// routines
unsigned int MemoryLeak(void);

void* operator new(size_t _size, char *_file, unsigned int _line);
void* operator new[](size_t _size, char *_file, unsigned int _line);


// class to detect the last destructor - then we start with
// memory leak checking
class _exit_detect
{
	public:
		
		static unsigned int count;
		_exit_detect(void) { ++count; }
		~_exit_detect(void) { if (--count == 0) MemoryLeak(); }
};

static _exit_detect _exit_counter;


// -----------------------------------------------------------------------------
// The Head of memory block
struct SMemList
{
	struct	SMemList *next, *prev;
	size_t 	size;
	bool	array;
	char 	*file;
	unsigned int line;
	int		tag;
};

const unsigned int __Tag = 0x0112358D;

// -----------------------------------------------------------------------------
// Macro with info about file and line where new occurs
#ifndef _FILE_CPP_DEBUG
#	define new		new(__FILE__, __LINE__)
#endif


// -----------------------------------------------------------------------------
// Memory:
//
//         ....
// |---------------------|
// |   SMemList block:   |
// | - next,prev...      |
// | - ...               |
// |---------------------|
// |   OBJ block         |  
// | ...                 |
// |---------------------|
//         ....
//
// 
// 1. When we create new object, we allocate memory for that object and
//    for SMemList block with info about size, pointers to next/prev 
//    elements of the list 
// 2. When we delete object, we delete object with its SMemList block
//    and update list of SMemLists
// 
#endif
#endif
