

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
	fclose(message);
	return SUCCESSCODE;
}

void hamming(string str)
{
	unsigned int hamming = 0, checkbits[5] = { 0 }, databits[16] = { 0 };
	hamming = hamming + (((unsigned int)str[0] & 0x1) << 3)
		+ (((unsigned int)str[0] & 0xE) << 4)
		+ (((unsigned int)str[0] & 0xF0) << 5) 
		+ (((unsigned int)str[1] & 0x7) << 12);
	printf("%d\n", ((int)str[0] & 0x1) << 3);
	printf("%d\n", str[0]);
	printf("%d\n", str[1]);
	printf("%d\n", hamming);
	for (int i = 0; i < 16; i++)
	{
		databits[i] = (hamming >> i) & 0x1;
	}
	checkbits[1] = databits[3] ^ databits[5] ^ databits[7] ^ databits[9] ^ databits[11] ^ databits[13] ^ databits[15];
	checkbits[2] = databits[3] ^ databits[6] ^ databits[7] ^ databits[10] ^ databits[11] ^ databits[14] ^ databits[15];
	checkbits[3] = databits[5] ^ databits[6] ^ databits[7] ^ databits[12] ^ databits[13] ^ databits[14] ^ databits[15];
	checkbits[4] = databits[9] ^ databits[10] ^ databits[11] ^ databits[12] ^ databits[13] ^ databits[14] ^ databits[15];
	hamming += (checkbits[1] << 1) + (checkbits[2] << 2) + (checkbits[3] << 4) + (checkbits[4] << 8);
	for (int i = 7; i >= 0; i--)
	{
		printf("%d", (str[0] >> i) & 0x1);
		if (i % 4 == 0)
			printf(" ");
	}
	printf("\n");
	for (int i = 15; i > 0; i--)
	{
		printf("%d", (hamming >> i) & 0x1);
		if (i % 4 == 0)
			printf(" ");
	}
	printf("\n");
	str[0] = hamming & 0xFF;
	str[1] = (hamming >> BYTESIZE) & 0xFF;
}
void logicshiftrightby11(string str)
{	
	int64_t word1 = 0, word2 = 0, mask1 = 0xFF, mask2 = 0x7;	
	for (int i = 0; i < 6; i++)
	{
		word1 += (int64_t)str[i] << (BYTESIZE * i);
		word2 += (int64_t)str[i + DATASIZE / 2] << (BYTESIZE * i);
	}	
	word2 = word2 << 16;
	word2 = word2 >> 20;
	for (int i = 0; i < 4; i++)
	{
		str[2 * i] = ((word1 >> (DATASIZE * i)) & mask1);
		str[2 * i + 1] = (char)((word1 >> (DATASIZE * i + BYTESIZE))& mask2);
		str[2 * i + BUFFER_SIZE_RCVR / 2] = (char)((word2 >> (DATASIZE * i))& mask1);
		str[2 * i + BUFFER_SIZE_RCVR / 2 + 1] = (char)((word2 >> (DATASIZE * i + BYTESIZE))& mask2);
	}		
}
