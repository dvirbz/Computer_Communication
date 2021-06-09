// Authors: Amit Zulan, Omer Miron//
// Handles Winsock Related Functions//

// Includes //
#include "WSA.h"
// Functions Impenmentation
BOOL Simple__Startup(WSADATA* startup)
{
	if (WSAStartup(MAKEWORD(2, 2), startup) != NO_ERROR)
	{
		printf(STARTUP_FAIL, WSAGetLastError());
		return ERRORCODE;
	}
	return TRUE;
}

SOCKADDR_IN Init__Address(string ip, int port)
{
	SOCKADDR_IN address = { 0 };
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip);
	address.sin_port = htons((u_short)port);
	return address;
}

int MC__Send(string ToSend, int BytesToSend, SOCKET sd, SOCKADDR* address)
{
	int bytes_transfered = 0, left_to_send = BytesToSend, addrsize = sizeof(SOCKADDR), total_sent = 0;
	while (left_to_send > 0)
	{
		bytes_transfered = sendto(sd, ToSend, left_to_send, 0, address, addrsize);
		if (SOCKET_ERROR == bytes_transfered)
		{
			printf("SendTo Failed, Error Code: %d\n", WSAGetLastError());
			return total_sent;
		}
		left_to_send -= bytes_transfered;
		ToSend += bytes_transfered;
		total_sent += bytes_transfered;
	}
	return total_sent;
}


