

#include "StructsAndEnums.h"
#include "Defines.h"
#include <string.h>
#include "WSA.h"
#include <inttypes.h>

void logicshiftrightby11(string str);
void hamming(string str);

int main(int argc, string* argv)
{
	assert(argc == NUM_OF_ARGC_SNDR);
	char buffer[BUFFER_SIZE_RCVR] = { 0 }, ip[MAX_IP_LEN] = { 0 }, filename[_MAX_PATH] = { 0 }, sndbuf[3] = { 0 };
	int port = 0;
	FILE* message = { 0 };
	WSADATA startup;
	SOCKADDR_IN address;
	SOCKET s;
	strcpy_s(ip, MAX_IP_LEN, argv[IP_SNDR]);
	strcpy_s(filename, _MAX_PATH, argv[FILENAME_SNDR]);
	port = strtol(argv[PORT_SNDR], NULL, 10);
	fopen_s(&message, filename, "rb");
	if (!simplestartup(&startup))
		return ERRORCODE;
	address = initaddress(ip, port);
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET)
		exit(ERRORCODE);
	fread(buffer, BUFFER_SIZE_SNDR - 1, 1, message);
	logicshiftrightby11(buffer);
	while (!feof(message))
	{
		for (int i = 0; i < 8; i++)
		{
			fprintf(stderr, "1 %d, %d, %s\n", *(buffer + 2 * i), *(buffer + 2 * i + 1), buffer);
			strncpy_s(sndbuf, sizeof(sndbuf), buffer + 2 * i, sizeof(sndbuf) - 1);
			fprintf(stderr, "2 %d, %d, %s\n", *sndbuf, *(sndbuf + 1), sndbuf);
			hamming(sndbuf);
			sendto(s, sndbuf, sizeof(sndbuf) - 1, 0, (SOCKADDR*)&address, sizeof(address));
		}
		fread(buffer, BUFFER_SIZE_SNDR - 1, 1, message);
		logicshiftrightby11(buffer);
	}
	return SUCCESSCODE;
}

void hamming(string str)
{
	//gj
}
void logicshiftrightby11(string str)
{	
	int64_t word1 = 0, word2 = 0, mask1 = 0xFF, mask2 = 0x7;	
	for (int i = 0; i < 6; i++)
	{
		word1 += (int64_t)str[i] << (8 * i);		
		word2 += (int64_t)str[i + 5] << (8 * i);		
	}	
	word2 = word2 << 16;
	word2 = word2 >> 20;
	for (int i = 0; i < 4; i++)
	{
		str[2 * i] = (char)((word1 >> (11 * i))& mask1);
		str[2 * i + 1] = (char)((word1 >> (11 * i + 8))& mask2);
		str[2 * i + 8] = (char)((word2 >> (11 * i))& mask1);
		str[2 * i + 9] = (char)((word2 >> (11 * i + 8))& mask2);
	}		
}
