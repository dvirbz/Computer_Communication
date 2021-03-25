//// Authers: Amit Zulan, Omer Miron//
//// Handles Inputs Outputs and Memory Allocation//
//
// Includes //
#include "IOMalloc.h"

BOOL stralloc(string* p_alloc, const int size, const alloctype allocationtype) //Allocated Memory and Initiallizes String
{
	switch (allocationtype)
	{
	case M:
		if (NULL == (*p_alloc = (string)malloc(size * sizeof(char))))
		{
			printf(MALLOCFAIL);
			free(*p_alloc);
			return FALSE;
		} break;
	case C:
		if (NULL == (*p_alloc = (string)calloc(size, sizeof(char))))
		{
			printf(MALLOCFAIL);
			free(*p_alloc);
			return FALSE;
		} break;
	case RE:
		if (NULL == (*p_alloc = (string)realloc(*p_alloc, size * sizeof(char))))
		{
			printf(MALLOCFAIL);
			free(*p_alloc);
			return FALSE;
		} break;
	default:
		printf(INVALIDALLOC);
		free(*p_alloc);
		return FALSE;
		break;
	}
	return TRUE;
}

BOOL intalloc(int** p_alloc, const int size, const alloctype allocationtype) //Allocated Memory and Initiallizes String
{
	switch (allocationtype)
	{
	case M:
		if (NULL == (*p_alloc = (int*)malloc(size * sizeof(int))))
		{
			printf(MALLOCFAIL);
			free(*p_alloc);
			return FALSE;
		} break;
	case C:
		if (NULL == (*p_alloc = (int*)calloc(size, sizeof(int))))
		{
			printf(MALLOCFAIL);
			free(*p_alloc);
			return FALSE;
		} break;
	case RE:
		if (NULL == (*p_alloc = (int*)realloc(*p_alloc, size * sizeof(int))))
		{
			printf(MALLOCFAIL);
			free(*p_alloc);
			return FALSE;
		} break;
	default:
		printf(INVALIDALLOC);
		free(*p_alloc);
		return FALSE;
		break;
	}
	return TRUE;
}

