// Authors: Amit Zulan, Omer Miron//
// Handles Winsock Related Functions//

// Includes //
#include "WSA.h"
// Functions Impenmentation
BOOL simplestartup(WSADATA* startup)
{
	if (WSAStartup(MAKEWORD(2, 2), startup) != NO_ERROR)
	{
		printf(STARTUP_FAIL, WSAGetLastError());
		return ERRORCODE;
	}
	return TRUE;
}

SOCKADDR_IN initaddress(string ip, int port)
{
	SOCKADDR_IN address = { 0 };
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip);
	address.sin_port = htons((u_short)port);
	return address;
}

//transfer_result SendBuffer(const char* Buffer, int BytesToSend, SOCKET sd)
//{
//	const char* CurPlacePtr = Buffer;
//	int BytesTransferred;
//	int RemainingBytesToSend = BytesToSend;
//
//	while (RemainingBytesToSend > 0)
//	{
//		/* send does not guarantee that the entire message is sent */
//		BytesTransferred = send(sd, CurPlacePtr, RemainingBytesToSend, 0);
//		if (BytesTransferred == SOCKET_ERROR)
//		{
//			printf("send() failed, error %d\n", WSAGetLastError());
//			return TRNS_FAILED;
//		}
//
//		RemainingBytesToSend -= BytesTransferred;
//		CurPlacePtr += BytesTransferred; // <ISP> pointer arithmetic
//	}
//
//	return TRNS_SUCCEEDED;
//}
//
//transfer_result ReceiveBuffer(char* OutputBuffer, int BytesToReceive, SOCKET sd)
//{
//	string CurPlacePtr = OutputBuffer;
//	int BytesJustTransferred;
//	int RemainingBytesToReceive = BytesToReceive;
//
//	while (RemainingBytesToReceive > 0)
//	{
//		/* send does not guarantee that the entire message is sent */
//		BytesJustTransferred = recv(sd, CurPlacePtr, RemainingBytesToReceive, 0);
//		if (BytesJustTransferred == SOCKET_ERROR)
//		{
//			printf("recv() failed, error %d\n", WSAGetLastError());
//			return TRNS_FAILED;
//		}
//		else if (BytesJustTransferred == 0)
//			return TRNS_DISCONNECTED; // recv() returns zero if connection was gracefully disconnected.
//
//		RemainingBytesToReceive -= BytesJustTransferred;
//		CurPlacePtr += BytesJustTransferred; // <ISP> pointer arithmetic
//	}
//
//	return TRNS_SUCCEEDED;
//}
//
//transfer_result SendString(const char* Str, SOCKET sd)
//{
//	/* Send the the request to the server on socket sd */
//	int TotalStringSizeInBytes;
//	transfer_result SendRes;
//
//	/* The request is sent in two parts. First the Length of the string (stored in
//	   an int variable ), then the string itself. */
//
//	TotalStringSizeInBytes = (int)(strlen(Str) + 1); // terminating zero also sent	
//
//	SendRes = SendBuffer(
//		(const char*)(&TotalStringSizeInBytes),
//		(int)(sizeof(TotalStringSizeInBytes)), // sizeof(int) 
//		sd);
//
//	if (SendRes != TRNS_SUCCEEDED) return SendRes;
//
//	SendRes = SendBuffer(
//		(const char*)(Str),
//		(int)(TotalStringSizeInBytes),
//		sd);
//
//	return SendRes;
//}
//
//transfer_result ReceiveString(char** OutputStrPtr, SOCKET sd)
//{
//	/* Recv the the request to the server on socket sd */
//	int TotalStringSizeInBytes;
//	transfer_result RecvRes;
//	char* StrBuffer = NULL;
//
//	if ((OutputStrPtr == NULL) || (*OutputStrPtr != NULL))
//	{
//		printf("The first input to ReceiveString() must be "
//			"a pointer to a char pointer that is initialized to NULL. For example:\n"
//			"\tchar* Buffer = NULL;\n"
//			"\tReceiveString( &Buffer, ___ )\n");
//		return TRNS_FAILED;
//	}
//
//	/* The request is received in two parts. First the Length of the string (stored in
//	   an int variable ), then the string itself. */
//
//	RecvRes = ReceiveBuffer(
//		(char*)(&TotalStringSizeInBytes),
//		(int)(sizeof(TotalStringSizeInBytes)), // 4 bytes
//		sd);
//
//	if (RecvRes != TRNS_SUCCEEDED) return RecvRes;
//
//	StrBuffer = (char*)malloc(TotalStringSizeInBytes * sizeof(char));
//
//	if (StrBuffer == NULL)
//		return TRNS_FAILED;
//
//	RecvRes = ReceiveBuffer(
//		(char*)(StrBuffer),
//		(int)(TotalStringSizeInBytes),
//		sd);
//
//	if (RecvRes == TRNS_SUCCEEDED)
//	{
//		*OutputStrPtr = StrBuffer;
//	}
//	else
//	{
//		free(StrBuffer);
//	}
//
//	return RecvRes;
//}
//
//BOOL breakmessage(str* message, str* rest)
//{
//	free(*rest), * rest = NULL;
//	str messageonly = { 0 };
//	if (!stralloc(&messageonly, strlen(*message) + 1, C) || !stralloc(rest, strlen(*message) + 1, C))
//		return FALSE;
//	int i = 0, j = 0;
//	while ((*message)[i] != ':' && (*message)[i] != STRCHAREND)
//	{
//		(messageonly)[i] = (*message)[i];
//		i++;
//	}
//	if ((*message)[i] != STRCHAREND)
//	{
//		(messageonly)[i] = CHARENDLINE;
//		(messageonly)[i + 1] = STRCHAREND;
//		i++;
//	}
//	else
//	{
//		free(*rest), * rest = NULL;
//		free(messageonly);
//		return TRUE;
//	}
//	while ((*message)[i] != STRCHAREND)
//	{
//		(*rest)[j] = (*message)[i];
//		i++;
//		j++;
//	}
//	(*rest)[j] = STRCHAREND;
//	if (!stralloc(&messageonly, strlen(messageonly) + 1, RE) || !stralloc(rest, strlen(*rest) + 1, RE))
//		return FALSE;
//	free(*message), *message = messageonly;
//	return TRUE;
//}
//
//BOOL breakresults(str* paramlist, str* param1, str* param2, str* param3, str* param4)
//{
//	if (!stralloc(param1, strlen((*paramlist)) + 1, C))
//		return FALSE;
//	int i = 0, j = 0;
//	while ((*paramlist)[i] != ';' && (*paramlist)[i] != STRCHAREND)
//	{
//		(*param1)[j] = (*paramlist)[i];
//		i++;
//		j++;
//	}
//	if ((*paramlist)[i] == STRCHAREND)
//	{
//		free(*paramlist), * paramlist = NULL;
//		if (!stralloc(param1, strlen(*param1) + 1, RE))
//			return FALSE;
//		return TRUE;
//	}
//	if (!stralloc(param2, strlen((*paramlist)) + 1, C))
//	{
//		free(*param1);
//		return FALSE;
//	}
//	j = 0, i++;
//	while ((*paramlist)[i] != ';' && (*paramlist)[i] != STRCHAREND)
//	{
//		(*param2)[j] = (*paramlist)[i];
//		i++;
//		j++;
//	}
//	if ((*paramlist)[i] == STRCHAREND)
//	{
//		free(*paramlist), * paramlist = NULL;
//		if (!stralloc(param2, strlen(*param2) + 1, RE))
//		{
//			free(*param1);
//			return FALSE;
//		}
//		return TRUE;
//	}
//	if (!stralloc(param3, strlen((*paramlist)) + 1, C))
//	{
//		free(*param1);
//		free(*param2);
//		return FALSE;
//	}
//	j = 0, i++;
//	while ((*paramlist)[i] != ';' && (*paramlist)[i] != STRCHAREND)
//	{
//		(*param3)[j] = (*paramlist)[i];
//		i++;
//		j++;
//	}
//	if ((*paramlist)[i] == STRCHAREND)
//	{
//		free(*paramlist), * paramlist = NULL;
//		if (!stralloc(param3, strlen(*param3) + 1, RE))
//		{
//			free(*param1);
//			free(*param2);
//			return FALSE;
//		}
//		return TRUE;
//	}
//	if (!stralloc(param4, strlen((*paramlist)) + 1, C))
//	{
//		free(*param1);
//		free(*param2);
//		free(*param3);
//		return FALSE;
//	}
//	j = 0, i++;
//	while ((*paramlist)[i] != ';' && (*paramlist)[i] != CHARENDLINE)
//	{
//		(*param4)[j] = (*paramlist)[i];
//		i++;
//		j++;
//	}
//	if (!stralloc(param4, strlen(*param4) + 1, RE))
//	{
//		free(*param1);
//		free(*param2);
//		free(*param3);
//		return FALSE;
//	}	free(*paramlist), * paramlist = NULL;
//	return TRUE;
//}

//BOOL gracefuldisconnection(SOCKET* sock)
//{
//	transfer_result res;
//	str message = { 0 };
//	shutdown(*sock, SD_SEND);
//	do
//	{
//		free(message);
//		message = NULL;
//		res = ReceiveString(&message, *sock);
//		if (res == TRNS_FAILED)
//			return FALSE;
//	} while (res != TRNS_DISCONNECTED);
//	if (closesocket(*sock))
//		printf(SOCKETFAIL);
//	*sock = INVALID_SOCKET;
//	return TRUE;
//}

