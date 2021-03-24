

#include "StructsAndEnums.h"
#include "Defines.h"
#include <string.h>
#include "WSA.h"
#include <conio.h>
#include <math.h>
#include <inttypes.h>

void logicshiftleftby11(string str);
int nonblockingrecv();
int unHamming(string str);
void Get__Data(string p, string data, int* currentbyte);
BOOL ispowof2(int num);

int main(int argc, string* argv)
{
	assert(argc == NUM_OF_ARGC_RCVR);
	char buffer[BUFFER_SIZE_RCVR] = "\0", filename[_MAX_PATH] = { 0 }, endbuf[5] = "\0", packet[PACKETSIZE] = { 0 };
	int port = 0, maxfd, n, temp = -1, currentbyte = 0, corrected = 0, addrsize = sizeof(SOCKADDR_IN);
	TIMEVAL waittime = { 0 };
	unsigned int keystrokes = 0;
	fd_set fds;
	FILE* message = { 0 };
	WSADATA startup;
	SOCKADDR_IN address_rcvr = { 0 }, address_sndr = { 0 };
	address_sndr.sin_port = 0;
	SOCKET s, channel;
	Summary sum = { 0, 0, 0 };
	waittime.tv_usec = 1000;
	strcpy_s(filename, _MAX_PATH, argv[FILENAME_RCVR]);
	port = strtol(argv[PORT_RCVR], NULL, 10);
	fopen_s(&message, filename, "wb");
	if (!simplestartup(&startup))
		return ERRORCODE;
	address_rcvr = initaddress(LOCALHOST, port);
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET)
		exit(ERRORCODE);
	maxfd = s;
	if (bind(s, (SOCKADDR*)&address_rcvr, addrsize) == SOCKET_ERROR)
		printf("%d", 1);
	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(s, &fds);

		select(maxfd + 1, &fds, NULL, NULL, &waittime);

		if (_kbhit())
		{
			scanf_s("%s", endbuf, 5);
			if (!strcmp(endbuf, END))
			{
				sprintf_s(buffer, sizeof(buffer), "%d, %d, %d\r\n\r\n", sum.received, sum.written, sum.errors);
				if (address_sndr.sin_port == 0)
					break;
				if ((channel = socket(AF_INET, SOCK_DGRAM, 0) == SOCKET_ERROR))
				{
					printf("Failed to Create Socket with Error Code %d\n", WSAGetLastError());
				}
				//if (bind(channel, (SOCKADDR*)&address_sndr, addrsize))
				//{
				//	printf("Failed to Bind Socket with Error Code %d\n", WSAGetLastError());
				//}
				if (SOCKET_ERROR == sendto(channel, buffer, strlen(buffer), 0, (SOCKADDR*)&address_sndr, &addrsize))
					printf("Failed to Send Data Through Socket with Error Code %d\n", WSAGetLastError());
				closesocket(channel);
				break;
			}
		}
		if (FD_ISSET(s, &fds))
		{
			currentbyte = recvfrom(s, packet, sizeof(packet) - 1, 0, (SOCKADDR*)&address_sndr, &addrsize);
			if (currentbyte == SOCKET_ERROR)
			{
				printf("RECV ERROR %d\n", WSAGetLastError());
			}
			sum.received += currentbyte;
			while (currentbyte != 0)
			{
				Get__Data(packet, buffer, &currentbyte);
				for (int i = 0; i < 16; i += 2)
				{
					sum.errors += unHamming(buffer + i);
				}
				logicshiftleftby11(buffer);
				sum.written += strlen(buffer);
				fprintf(message, "%s", buffer);

			}
			//int x = (sum.written / 75.9) * 10;
			//if (x != temp)
			//{
			//	printf("%.2f%%\n", sum.written / 75.9);
			//	printf("corrected %d errors\n", corrected);
			//}
			//temp = x;
		}
	}
	fclose(message);
	closesocket(s);
	return SUCCESSCODE;
}


void logicshiftleftby11(string str)
{
	unsigned long long word1 = 0, word2 = 0, mask1 = 0xFF, mask2 = 0x7;
	unsigned int shift = 0;
	for (int i = 0; i < 8; i++)
	{
		word1 += ((unsigned long long)str[i] + 256) % 256 << shift;
		word2 += ((unsigned long long)str[i + 8] + 256) % 256 << shift;
		shift += ((i % 2) ? 3 : 8);
	}
	word1 += (word2 & 0xF) << 44;
	word2 >>= 4;	
	for (int i = 0; i < 6; i++)
	{
		str[i] = (word1 >> (i * 8)) & 0xFF;
		str[i + 6] = (word2 >> (i * 8)) & 0xFF;
	}
	str[11] = '\0';
}

BOOL ispowof2(int num)
{
	int i = 1;
	while (i <= num)
	{
		if (i == num)
			return TRUE;
		i *= 2;
	}
	return FALSE;
}

int unHamming(string str)
{
	unsigned int unhamming = 0, databits[16] = { 0 }, checkbits[5] = { 0 }, j = 0;
	int check = 0;
	unhamming = (unsigned int)str[0] & 0xFF;
	unhamming += (((unsigned int)str[1]) << BYTESIZE) & 0xFF00;
	for (int i = 0; i < 16; i++)
	{
		databits[i] = ((unhamming >> i) & 0x1);
	}
	for (int i = 0; i < 4; i++)
	{
		checkbits[i + 1] = (unhamming >> (int)pow(2, i)) & 0x1;
	}

	// check errors

	check = checkbits[1] ^ databits[3] ^ databits[5] ^ databits[7] ^ databits[9] ^ databits[11] ^ databits[13] ^ databits[15];
	check += (checkbits[2] ^ databits[3] ^ databits[6] ^ databits[7] ^ databits[10] ^ databits[11] ^ databits[14] ^ databits[15]) << 1;
	check += (checkbits[3] ^ databits[5] ^ databits[6] ^ databits[7] ^ databits[12] ^ databits[13] ^ databits[14] ^ databits[15]) << 2;
	check += (checkbits[4] ^ databits[9] ^ databits[10] ^ databits[11] ^ databits[12] ^ databits[13] ^ databits[14] ^ databits[15]) << 3;
	if (check != 0)
	{
		databits[check] = 1 - databits[check];
	}

	// check errors
	str[0] = 0;
	str[1] = 0;
	for (int i = 1; i < 16; i++)
	{
		if (!ispowof2(i))
		{
			if (i == 13)
				j = 0;
			if (i < 13)
				str[0] += (databits[i] << j);
			else
				str[1] += (databits[i] << j);
			j++;
		}
	}
	return (check != 0);
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
		temp = p[i + 8] & mask1;
		word2 += temp << (shift + 4);

	}
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

//for (int i = 15; i > 0; i--)
//{
//	printf("%d", (unhamming >> i) & 0x1);
//	if (i % 4 == 0)
//		printf(" ");
//}
//printf("\n");