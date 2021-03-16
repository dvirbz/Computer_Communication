

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
	char buffer[BUFFER_SIZE_RCVR] = { 0 }, ip[MAX_IP_LEN] = { 0 }, filename[_MAX_PATH] = { 0 }, sndbuf[3] = { 0 }, sndstr[321] = { 0 };
	int port = 0, blocki = 0;
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
	while (!feof(message))
	{
		logicshiftrightby11(buffer);
		for (int i = 0; i < 8; i++)
		{
			strncpy_s(sndbuf, sizeof(sndbuf), buffer + 2 * i, sizeof(sndbuf) - 1);
			//printf("1. sndbuf is: %s, %d\n", sndbuf, strlen(sndbuf));
			hamming(sndbuf);
			//printf("2. sndbuf is: %s, %d\n", sndbuf, strlen(sndbuf));
			//snprintf(sndstr, sizeof(sndbuf) + sizeof(sndstr), "%s%s", sndstr, sndbuf);
			//printf("sndstr is: %s, %d, %s, %d\n", sndstr, strlen(sndstr), sndbuf, strlen(sndbuf));
			//blocki++;
			//blocki = 0;
			blocki += sendto(s, sndbuf, sizeof(sndbuf) - 1, 0, (SOCKADDR*)&address, sizeof(address));
			sndstr[0] = '\0';
		}
		fread(buffer, BUFFER_SIZE_SNDR - 1, 1, message);
	}
	printf("blocki: %d\n", blocki);
	//if (blocki != 0)
	//{
	//	sndstr[2 * blocki + 1] = '\0';
	//	//printf("%d", strlen(sndstr));
	//	sendto(s, sndstr, sizeof(sndstr) - 1, 0, (SOCKADDR*)&address, sizeof(address));
	//}
	fclose(message);
	return SUCCESSCODE;
}

void hamming(string str)
{
	unsigned int hamming = 0, checkbits[5] = { 0 }, databits[16] = { 0 };
	hamming = hamming + (((unsigned int)str[0] & 0x1) << 3)
		+ (((unsigned int)str[0] & 0xE) << 4)
		+ (((unsigned int)str[0] & 0xF0) << 5) 
		+ (((unsigned int)str[1] & 0x7) << 13);
	for (int i = 0; i < 16; i++)
	{
		databits[i] = (hamming >> i) & 0x1;
	}
	checkbits[1] = databits[3] ^ databits[5] ^ databits[7] ^ databits[9] ^ databits[11] ^ databits[13] ^ databits[15];
	checkbits[2] = databits[3] ^ databits[6] ^ databits[7] ^ databits[10] ^ databits[11] ^ databits[14] ^ databits[15];
	checkbits[3] = databits[5] ^ databits[6] ^ databits[7] ^ databits[12] ^ databits[13] ^ databits[14] ^ databits[15];
	checkbits[4] = databits[9] ^ databits[10] ^ databits[11] ^ databits[12] ^ databits[13] ^ databits[14] ^ databits[15];
	hamming += (checkbits[1] << 1) + (checkbits[2] << 2) + (checkbits[3] << 4) + (checkbits[4] << 8);
	str[0] = hamming & 0xFF;
	str[1] = (hamming >> BYTESIZE) & 0xFF;
	str[2] = '\0';
}
void logicshiftrightby11(string str)
{	
	unsigned long long word1 = 0, word2 = 0, mask1 = 0xFF, mask2 = 0x7;	
	for (int i = 0; i < 6; i++)
	{
		word1 += (unsigned long long)str[i] << (BYTESIZE * i);
		word2 += (unsigned long long)str[i + DATASIZE / 2] << (BYTESIZE * i);
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
