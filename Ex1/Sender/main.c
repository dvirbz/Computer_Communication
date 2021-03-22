

#include "StructsAndEnums.h"
#include "Defines.h"
#include <string.h>
#include "WSA.h"
#include <inttypes.h>
#include "IOMalloc.h"


void logicshiftrightby11(string str);
void hamming(string str);
void Add__Data(string p, string data, int* currentbyte);
void Get__Data(string p, string data, int* currentbyte);


int main(int argc, string* argv)
{
	assert(argc == NUM_OF_ARGC_SNDR);
	char buffer[BUFFER_SIZE_RCVR] = { 0 }, ip[MAX_IP_LEN] = { 0 }, filename[_MAX_PATH] = { 0 }, packet[PACKETSIZE] = { 0 };
	int port = 0, currentbyte = 0;
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
		for (int i = 0; i < BUFFER_SIZE_RCVR - 1; i += 2)
		{
			hamming(buffer + i);
		}
		buffer[16] = '\0';
		Add__Data(packet, buffer, &currentbyte);
		if (currentbyte == PACKETSIZE - 1)
		{
			sendto(s, packet, currentbyte, 0, (SOCKADDR*)&address, sizeof(address));
			Sleep(1);
			currentbyte = 0;
			for (int i = 0; i < sizeof(packet); i++)
			{
				packet[i] = '\0';
			}
		}
		for (int i = 0; i < sizeof(buffer); i++)
		{
			buffer[i] = '\0';
		}
		fread(buffer, BUFFER_SIZE_SNDR - 1, 1, message);
	}
	//if (currentbyte != 0)
	//{
	//	string quicksend = { 0 };
	//	if (stralloc(&quicksend, currentbyte, C))
	//	{
	//		strcpy_s(quicksend, currentbyte, packet);
	//		quicksend[currentbyte] = '\0';
	//		sendto(s, quicksend, currentbyte, 0, (SOCKADDR*)&address, sizeof(address));
	//		free(quicksend);
	//	}
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

void Add__Data(string p, string data, int* currentbyte)
{
	unsigned long long word1 = 0, word2 = 0, cmask, transfermask, temp = 0, compliment = 0;
	unsigned int byte_transfer;
	cmask = (unsigned long long)(pow(2, BYTESIZE) - 1);
	byte_transfer = 4 + BYTESIZE;
	transfermask = (unsigned long long)(pow(2, byte_transfer) - 1);
	for (int i = 0; i < 8; i += 2)
	{
		temp = (unsigned long long)(((data[i] >> 1) & 0x7F) + (data[i + 1] << (BYTESIZE - 1)));
		word1 += ((temp & 0x7FFF) << (HAMMINGSIZE * (i / 2)));
		temp = (unsigned long long)(((data[i + 8] >> 1) & 0x7F) + (data[i + 9] << (BYTESIZE - 1)));
		word2 += ((temp & 0x7FFF) << (HAMMINGSIZE * (i / 2)));
	}
	compliment = cmask & word1;
	word1 >>= BYTESIZE;
	p[*currentbyte] += compliment;
	word1 += ((word2 & transfermask) << (64 - byte_transfer));
	word2 >>= byte_transfer;
	(*currentbyte)++;
	for (int i = 0; i < 8; i++)
	{
		p[*currentbyte + i] = (char)((word1 >> (BYTESIZE * i)) & 0xFF);
		p[*currentbyte + i + 8] = (char)((word2 >> (BYTESIZE * i)) & 0xFF);
	}
	(*currentbyte) += 14;
}

void Get__Data(string p, string data, int* currentbyte)
{
	unsigned long long word1 = 0, word2 = 0, mask1 = 0xFF, mask3 = 0x7F, mask2 = 0xF0;
	unsigned int shift = 0;
	word2 = ((unsigned long long)p[7] & mask2) >> 4;
	for (int i = 0; i < 8; i++)
	{
		shift = 8 * i;
		unsigned long long temp = p[i] & mask1;
		word1 += temp << shift;
		//printf("Word1 -> ");
		//for (int j = 63; j >= 0; j--)
		//{
		//	printf("%d", (word1 >> j) & 0x1);
		//	if (j % 4 == 0)
		//		printf(" ");
		//}
		//printf("\n");
		temp = p[i + 8] & mask1;
		word2 += temp << (shift + 4);

	}
	//printf("\n\n\n");
	for (int i = 0; i < 4; i++)
	{
		word1 = word1 >> ((i == 0) ? 0 : 8);
		data[2 * i] = (word1 & mask3) << 1;
		word1 = word1 >> 7;
		data[2 * i + 1] = word1 & mask1;

		word2 = word2 >> ((i == 0) ? 0 : 8);
		data[2 * i + 8] = (word2 & mask3) << 1;
		word2 = word2 >> 7;
		data[2 * i + 9] = word2 & mask1;
	}
	data[16] = '\0';

	for (int i = 0; i < *currentbyte - 15; i++)
	{
		p[i] = p[i + 15];
	}
	for (int i = *currentbyte - 15; i < *currentbyte; i++)
	{
		p[i] = '\0';
	}
	*currentbyte -= 15;
}

//for (int j = 7; j >= 0; j--)
//{
//	printf("%d", (buffer[i + 1] >> j) & 0x1);
//	if (j % 4 == 0)
//		printf(" ");
//}
//for (int j = 7; j > 0; j--)
//{
//	printf("%d", (buffer[i] >> j) & 0x1);
//	if (j % 4 == 0)
//		printf(" ");
//}
//printf("\n");

//for (int i = 15; i >= 0; i -= 2)
//{
//	for (int j = 7; j >= 0; j--)
//	{
//		printf("%d", (buffer[i] >> j) & 0x1);
//	}
//	for (int j = 7; j > 0; j--)
//	{
//		printf("%d", (buffer[i - 1] >> j) & 0x1);
//	}
//	printf(" ");
//}
//printf("\n");
//printf("\n");
//Add__Data(packet, buffer, 0);
//for (int i = 15; i >= 0; i -= 2)
//{
//	for (int j = 7; j >= 0 && i != 15; j--)
//	{
//		printf("%d", (packet[i] >> j) & 0x1);
//		if ((i * 8 + j) % 15 == 0)
//			printf(" ");
//	}
//	for (int j = 7; j >= 0; j--)
//	{
//		printf("%d", (packet[i - 1] >> j) & 0x1);
//	}
//}
//printf("\n");
//printf("\n");