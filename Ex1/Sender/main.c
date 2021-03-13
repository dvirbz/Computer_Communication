

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
	int port = 0, temp;
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
			fprintf(stderr, "%d, %d, %s\n", 1, *buffer, buffer);
			strncpy_s(sndbuf, sizeof(sndbuf), buffer + 2 * i, sizeof(sndbuf) - 1);
			fprintf(stderr, "%d, %d, %s\n", 2, *sndbuf, sndbuf);
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

}
void logicshiftrightby11(string str)
{
	/*int temp;
	for (int i = 0; i < strlen(str) - 1; i++)
	{
		str[i] = str[i + 1];
	}
	for (int i = 1; i < strlen(str); i++)
	{
		temp = str[i] & 0x7;
		temp = temp << 5;
		str[i - 1] = str[i - 1] >> 3;
		str[i - 1] |= temp;
	}*/
	unsigned long int word1 = 0, word2 = 0, mask1 = 0xFF, mask2 = 0x7;
	for (int i = 0; i < 1; i++)
	{
		word1 += str[i] << (8 * i);
		printf("%d\n", str[i]);
		word2 += str[i + 5] << (8 * i);
		str[i] = '\0';
	}
	word2 = word2 << 16;
	word2 = word2 >> 20;
	for (int i = 0; i < 64; i++)
	{
		printf("%d", ((word2 << i) >> 63 ));
		if ((i + 1) % 4 == 0)
			printf(" ");
	}
	printf("\n");

	for (int i = 0; i < 64; i++)
	{
		printf("%d", ((word1 << i) >> 63));
		if ((i + 1) % 4 == 0)
			printf(" ");
	}
	printf("\n");
	Sleep(15000);
	for (int i = 0; i < 4; i++)
	{
		str[2 * i] = word1 & mask1;
		str[2 * i + 1] = (word1 & (mask2 << (i * 11))) >> (11 * i + 8);
		str[2 * i + 8] = word2 & mask1;
		str[2 * i + 9] = (word2 & (mask2 << (i * 11))) >> (11 * i + 8);
	}
}