//// Authers: Amit Zulan, Omer Miron//
//// Handles Inputs Outputs and Memory Allocation//
//
// Includes //
#include "IOMalloc.h"
//
//// Functions Impenmentation
//BOOL openfile(FILE** F, const stringfilename, const filemode mode) //Opens File
//{
//	switch (mode)
//	{
//	case R:
//		if (fopen_s(F, filename, READ))
//		{
//			printf(FILEOPENFAIL);
//			return FALSE;
//		} break;
//	case W:
//		if (fopen_s(F, filename, WRITE))
//		{
//			printf(FILEOPENFAIL);
//			return FALSE;
//		} break;
//	case A:
//		if (fopen_s(F, filename, APPEND))
//		{
//			printf(FILEOPENFAIL);
//			return FALSE;
//		} break;
//	default:
//		printf(INVALIDMODE);
//		return FALSE;
//		break;
//	}
//	return TRUE;
//}
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

//BOOL wordalloc(DWORD** p_alloc, const int size, const alloctype allocationtype) //Allocated Memory and Initiallizes String
//{
//	switch (allocationtype)
//	{
//	case M:
//		if (NULL == (*p_alloc = (DWORD*)malloc(size * sizeof(DWORD))))
//		{
//			printf(MALLOCFAIL);
//			free(*p_alloc);
//			return FALSE;
//		} break;
//	case C:
//		if (NULL == (*p_alloc = (DWORD*)calloc(size, sizeof(DWORD))))
//		{
//			printf(MALLOCFAIL);
//			free(*p_alloc);
//			return FALSE;
//		} break;
//	case RE:
//		if (NULL == (*p_alloc = (DWORD*)realloc(*p_alloc, size * sizeof(DWORD))))
//		{
//			printf(MALLOCFAIL);
//			free(*p_alloc);
//			return FALSE;
//		} break;
//	default:
//		printf(INVALIDALLOC);
//		free(*p_alloc);
//		return FALSE;
//		break;
//	}
//	return TRUE;
//}
//BOOL handlealloc(HANDLE** p_alloc, const int size, const alloctype allocationtype)
//{
//	switch (allocationtype)
//	{
//	case M:
//		if (NULL == (*p_alloc = (HANDLE*)malloc(size * sizeof(HANDLE))))
//		{
//			printf(MALLOCFAIL);
//			free(*p_alloc);
//			return FALSE;
//		} break;
//	case C:
//		if (NULL == (*p_alloc = (HANDLE*)calloc(size, sizeof(HANDLE))))
//		{
//			printf(MALLOCFAIL);
//			free(*p_alloc);
//			return FALSE;
//		} break;
//	case RE:
//		if (NULL == (*p_alloc = (HANDLE*)realloc(*p_alloc, size * sizeof(HANDLE))))
//		{
//			printf(MALLOCFAIL);
//			free(*p_alloc);
//			return FALSE;
//		} break;
//	default:
//		printf(INVALIDALLOC);
//		free(*p_alloc);
//		return FALSE;
//		break;
//	}
//	return TRUE;
//}
//BOOL finddirectory(const string filename, string directory)
//{ 
//	if (filename == NULL)
//		return FALSE;
//	int len = strlen(filename);
//	strcpy_s(directory, len + 1, filename);
//	for (int i = 1; i <= len; i++)
//	{
//		if (filename[len - i] == BACKSLASH)
//		{
//			directory[len - i + 1] = STRCHAREND;
//			return TRUE;
//		}
//	}
//	return FALSE;
//}