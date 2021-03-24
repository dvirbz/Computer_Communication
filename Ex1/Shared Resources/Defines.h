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

#define NUM_OF_ARGC_SNDR 4
#define NUM_OF_ARGC_RCVR 3
#define NUM_OF_ARGC_CHNL 6
#define MAX_IP_LEN 15
#define BUFFER_SIZE_SNDR 12
#define STARTUP_FAIL "WSAStartup() Failed with Error: %d\n"
#define SUCCESSCODE 0
#define ERRORCODE -1
#define LOCALHOST "127.0.0.1"
#define STDIN 0
#define END "End"
#define BUFFER_SIZE_RCVR 17
#define BYTESIZE 8
#define DATASIZE (BUFFER_SIZE_SNDR - 1)
#define HAMMINGSIZE 15
#define PACKETSIZE 1501
#define MALLOCFAIL "Failed To Allocate Memory.\n"
#define INVALIDALLOC "Invalid Allocation type.\n"
#define K_INIT 14


#endif	// DEFINES_H