

#include "StructsAndEnums.h"
#include "Defines.h"
#include <string.h>
#include "WSA.h"


int main(int argc, string* argv)
{
	assert(argc == NUM_OF_ARGC);
	char buffer[BUFFER_SIZE] = { 0 }, ip[MAX_IP_LEN] = { 0 }, filename[_MAX_PATH] = { 0 };
	int port = 0;
	FILE* message = { 0 };
	WSADATA startup;
	SOCKADDR_IN address;
	SOCKET s;
	strcpy_s(ip, MAX_IP_LEN, argv[IP]);
	strcpy_s(filename, _MAX_PATH, argv[FILENAME]);
	port = strtol(argv[PORT], NULL, 10);
	fopen_s(&message, filename, "rb");
	if (!simplestartup(&startup))
		return ERRORCODE;
	address = initaddress(ip, port);
	s = socket(AF_INET, SOCK_DGRAM, 0);
	fread(buffer, BUFFER_SIZE - 1, 1, message);
	sendto(s, buffer, BUFFER_SIZE - 1, 0, (SOCKADDR*)&address, sizeof(address));


	
	


}