// Authors: Amit Zulan, Omer Miron//
// Handles Winsock Related Functions//

#ifndef WSA_H
#define WSA_H
#define _WINSOCK_DEPRECATED_NO_WARNINGS


// Includes //
#include <WinSock2.h>
#include "Defines.h"
#include "StructsAndEnums.h"

// Function Declerations //
BOOL simplestartup(WSADATA* startup);

SOCKADDR_IN initaddress(string ip, int port);

transfer_result SendBuffer(const char* Buffer, int BytesToSend, SOCKET sd);

transfer_result ReceiveBuffer(char* OutputBuffer, int RemainingBytesToReceive, SOCKET sd);

transfer_result SendString(const char* Str, SOCKET sd);

transfer_result ReceiveString(char** OutputStrPtr, SOCKET sd);

BOOL breakmessage(string* message, string* rest);

BOOL breakresults(string* paramlist, string* param1, string* param2, string* param3, string* param4);

BOOL gracefuldisconnection(SOCKET* sock);

#endif // WSA_H