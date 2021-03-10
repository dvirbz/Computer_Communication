/* Owners: Dvir Ben Zikri
		   Amit Zulan     */

#ifndef DEFINES_H
#define DEFINES_H

   // includes //
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "tchar.h"
#include "math.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "io.h"
#pragma warning(disable: 6387)

typedef char* string;

#define NUM_OF_ARGC 4
#define MAX_IP_LEN 15
#define BUFFER_SIZE 12
#define STARTUP_FAIL "WSAStartup() Failed with Error: %d\n"
#define SUCCESSCODE 0
#define ERRORCODE -1



#endif	// DEFINES_H