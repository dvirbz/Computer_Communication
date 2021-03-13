// Authers: Amit Zulan, Omer Miron//
// Definitions of Structs and Enums //
#ifndef STRUCTSANDENUMS_H
#define STRUCTSANDENUMS_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#pragma comment(lib, "Ws2_32.lib")

	
	


// Enums //
typedef enum { M, C, RE } alloctype; // memory allocation type.
typedef enum { TRNS_FAILED, TRNS_DISCONNECTED, TRNS_SUCCEEDED } transfer_result;
typedef enum { SUCCEEDED, CONTINUE, TIMEO, FAILED } whatevent; // check event
typedef enum { IP_SNDR = 1, PORT_SNDR, FILENAME_SNDR } arguments_SNDR; 
typedef enum { PORT_RCVR = 1, FILENAME_RCVR } arguments_SNDR;


#endif // STRUCTS	ANDENUMS_H