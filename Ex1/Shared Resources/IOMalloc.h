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
//BOOL openfile(FILE** F, const stringfilename, const filemode mode); 
////Opens File With Specified mode (R, W, A). 
////Inputs: F - address to store file pointer in. filename - name of the file. mode - (Read, Write, Append). Output: BOOL type for failure check
//
BOOL stralloc(string* p_alloc, const int size, const alloctype allocationtype); 
//Memory Allocation For Strings With Specified Allocation Type.
//Output: BOOL type for failure check. Inputs: p_alloc - address to allocate memory in. size - number of elements in memory allocation. alloctype - type of allocation (Malloc, Calloc, Realloc).

//BOOL intalloc(int** p_alloc, const int size, const alloctype allocationtype); 
////Memory Allocation For Integer Arrays With Specified Allocation Type. 
////Output: BOOL type for failure check. Inputs: p_alloc - address to allocate memory in. size - number of elements in memory allocation. alloctype - type of allocation (Malloc, Calloc, Realloc).
//
//BOOL handlealloc(HANDLE** p_alloc, const int size, const alloctype allocationtype); 
////Memory Allocation For Handle Arrays With Specified Allocation Type. Output: BOOL type for failure check
////Output: BOOL type for failure check. Inputs: p_alloc - address to allocate memory in. size - number of elements in memory allocation. alloctype - type of allocation (Malloc, Calloc, Realloc).
//
//BOOL wordalloc(DWORD** p_alloc, const int size, const alloctype allocationtype); 
////Memory Allocation For DWORD Arrays With Specified Allocation Type. Output: BOOL type for failure check
////Output: BOOL type for failure check. Inputs: p_alloc - address to allocate memory in. size - number of elements in memory allocation. alloctype - type of allocation (Malloc, Calloc, Realloc).
//
//BOOL finddirectory(const string filename, string directory); 
////Finds The Directory Of Specified File Name. 
////Output: BOOL type for failure check. Inputs: filename - the function finds its directory. directory - updated to filename's containing folder.
//
//
//
//
#endif // IOMALLOC_H