// Authers: Amit Zulan, Omer Miron//
// Handles Inputs Outputs and Memory Allocation //
#ifndef IOMALLOC_H
#define IOMALLOC_H

// includes //
#include "crtdbg.h"
#include "StructsAndEnums.h"
#include <winsock2.h>
#include "Defines.h"


//// Function Declerations //

BOOL stralloc(string* p_alloc, const int size, const alloctype allocationtype); 
//Memory Allocation For Strings With Specified Allocation Type.
//Output: BOOL type for failure check. Inputs: p_alloc - address to allocate memory in. size - number of elements in memory allocation. alloctype - type of allocation (Malloc, Calloc, Realloc).

BOOL intalloc(int** p_alloc, const int size, const alloctype allocationtype); 
//Memory Allocation For Integer Arrays With Specified Allocation Type. 
//Output: BOOL type for failure check. Inputs: p_alloc - address to allocate memory in. size - number of elements in memory allocation. alloctype - type of allocation (Malloc, Calloc, Realloc).

#endif // IOMALLOC_H