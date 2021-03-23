// Authers: Amit Zulan, Omer Miron//
// Definitions of Structs and Enums //
#ifndef STRUCTSANDENUMS_H
#define STRUCTSANDENUMS_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#pragma comment(lib, "Ws2_32.lib")

// Structs //
typedef char* string;
	
	


// Enums //
typedef enum { M, C, RE } alloctype; // memory allocation type.
typedef enum { TRNS_FAILED, TRNS_DISCONNECTED, TRNS_SUCCEEDED } transfer_result;
typedef enum { SUCCEEDED, CONTINUE, TIMEO, FAILED } whatevent; // check event
typedef enum { IP_SNDR = 1, PORT_SNDR, FILENAME_SNDR } arguments_SNDR; 
typedef enum { PORT_RCVR = 1, FILENAME_RCVR } arguments_RCVR;
typedef enum { PORT_CHNL = 1, IP_RCVR, PORT_TO_RCVR, RND_CHANCE, SEED} arguments_CHNL;



#endif // STRUCTSANDENUMS_H