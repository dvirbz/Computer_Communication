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
BOOL Simple__Startup(WSADATA* startup);

SOCKADDR_IN Init__Address(string ip, int port);

int MC__Send(string ToSend, int size, SOCKET sd, SOCKADDR* address);

#endif // WSA_H