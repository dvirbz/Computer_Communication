

#include "StructsAndEnums.h"
#include "Defines.h"
#include <string.h>
#include "WSA.h"
#include <conio.h>
#include <math.h>
#include <inttypes.h>

FILE* Init(int* port, string* argv);
void logicshiftleftby11(string str);
int unHamming(string str);
int Concat_unHamming(string buffer);
void Get_Data(string p, string data, int* currentbyte);
BOOL ispowof2(int num);
void Receive_Packet(SOCKET sd_read, SOCKADDR* address, FILE* message, Summary* sum);
void Final_message(Summary sum, SOCKET sd_write, SOCKADDR* address);


int main(int argc, string* argv)
{
	assert(argc == NUM_OF_ARGC_RCVR);
	char endbuf[5] = "\0", packet[PACKETSIZE] = { 0 };
	int port = 0, maxfd, n, temp = -1, currentbyte = 0, corrected = 0, addrsize = sizeof(SOCKADDR_IN);
	TIMEVAL waittime = { .tv_sec = 0, .tv_usec = 1000 };
	unsigned int keystrokes = 0;
	fd_set fds_read, fds_write;
	FILE* message = { 0 };
	SOCKADDR_IN address_rcvr = { 0 }, address_sndr = { 0 };
	address_sndr.sin_port = 0;
	SOCKET sd_read;
	Summary sum = { 0, 0, 0 };

	if (NULL == (message = Init(&port, argv)))
		return ERRORCODE;

	address_rcvr = Init__Address(LOCALHOST, port);

	if (INVALID_SOCKET == (sd_read = socket(AF_INET, SOCK_DGRAM, 0)))
	{
		printf("Failed To Open Sockets, Error Code is: %d\n", WSAGetLastError());
		exit(ERRORCODE);
	}

	maxfd = sd_read;
	if (bind(sd_read, (SOCKADDR*)&address_rcvr, addrsize))
	{
		printf("Failed to Bind Socket with Error Code %d\n", WSAGetLastError());
		exit(ERRORCODE);
	}

	while (1)
	{
		FD_ZERO(&fds_read);
		FD_SET(sd_read, &fds_read);

		select(maxfd + 1, &fds_read, NULL, NULL, &waittime);

		if (_kbhit())
		{
			scanf_s("%s", endbuf, 5);
			if (!strcmp(endbuf, END))
			{
				Final_message(sum, sd_read, (SOCKADDR*)&address_sndr);
				break;
			}
		}
		if (FD_ISSET(sd_read, &fds_read))
		{
			Receive_Packet(sd_read, (SOCKADDR*)&address_sndr, message, &sum);
			int x = (sum.written / 4485646.11) * 10;
			if (x != temp)
			{
				printf("%.2f%%\n", sum.written / 4485646.11);
			}
			temp = x;
		}
	}
	fclose(message);
	if (closesocket(sd_read))
		printf("Failed To Close Socket\n");
	return SUCCESSCODE;
}

FILE* Init(int* port, string* argv)
{
	FILE* message = { 0 };
	char filename[_MAX_PATH] = { 0 };
	WSADATA startup;
	strcpy_s(filename, _MAX_PATH, argv[FILENAME_RCVR]);
	*port = strtol(argv[PORT_RCVR], NULL, 10);
	fopen_s(&message, filename, "wb");
	if (!Simple__Startup(&startup))
		return NULL;
	return message;
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
	return (check != 0 && !ispowof2(check));
  }

int Concat_unHamming(string buffer)
{
	int count = 0;
	for (int i = 0; i < BUFFER_SIZE_RCVR - 1; i += 2)
	{
		count += unHamming(buffer + i);
	}
	buffer[BUFFER_SIZE_RCVR - 1] = '\0';
	return count;
}

void Get_Data(string p, string data, int* currentbyte)
{
	unsigned long long word1 = 0, word2 = 0, mask1 = 0xFF, mask3 = 0x7F, mask2 = 0xF0;
	unsigned int shift = 0;
	word2 = ((unsigned long long)(p[7]) & mask2) >> 4;
	for (int i = 0; i < 8; i++)
	{
		shift = 8 * i;
		unsigned long long temp = (unsigned long long)(p[i]) & mask1;
		word1 += temp << shift;
		temp = (unsigned long long)(p[i + 8]) & mask1;
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

void Receive_Packet(SOCKET sd_read, SOCKADDR* address, FILE* message, Summary* sum)
{
	int addrsize = sizeof(SOCKADDR), currentbyte;
	char packet[PACKETSIZE] = { 0 };
	currentbyte = recvfrom(sd_read, packet, sizeof(packet) - 1, 0, address, &addrsize);
	if (currentbyte == SOCKET_ERROR)
	{
		printf("RECV ERROR %d\n", WSAGetLastError());
	}
	char buffer[BUFFER_SIZE_RCVR] = { 0 };
	int counter = 0;
	sum->received += currentbyte;
	while (currentbyte != 0)
	{
		Get_Data(packet, buffer, &currentbyte);
		sum->errors += Concat_unHamming(buffer);
		logicshiftleftby11(buffer);
		sum->written += BUFFER_SIZE_SNDR - 1;
		fwrite(buffer, 1, BUFFER_SIZE_SNDR - 1, message);
	}
}

void Final_message(Summary sum, SOCKET sd_write, SOCKADDR* address)
{
	char packet[40] = { 0 };
	int addrsize = sizeof(SOCKADDR);
	sprintf_s(packet, sizeof(packet) - 1, "%d, %d, %d\r\n\r\n", sum.received, sum.written, sum.errors);
	printf("received: %d\nwrote: %d\ndetected & corrected %d errors\n", sum.received, sum.written, sum.errors);
	char ip[MAX_IP_LEN];
	if (SOCKET_ERROR == sendto(sd_write, packet, strlen(packet), 0, address, addrsize))
		printf("Failed to Send Data Through Socket with Error Code %d\n", WSAGetLastError());
}