

#include "StructsAndEnums.h"
#include "Defines.h"
#include <string.h>
#include "WSA.h"
#include <conio.h>
#include <math.h>
#include <inttypes.h>

void logicshiftleftby11(string str);
int nonblockingrecv();
void unhamming(string str);

int main(int argc, string* argv)
{
	assert(argc == NUM_OF_ARGC_RCVR);
	char buffer[BUFFER_SIZE_RCVR] = "\0", filename[_MAX_PATH] = { 0 }, rcvbuf[3] = { 0 }, endbuf[5] = "\0";
	int port = 0, maxfd, n;
	TIMEVAL waittime = { 0 };
	unsigned int keystrokes = 0, sll_i = 0;
	fd_set fds;
	FILE* message = { 0 };
	WSADATA startup;
	SOCKADDR_IN address;
	SOCKET s;
	int senderaddrsize = sizeof(address);
	waittime.tv_usec = 1000;
	strcpy_s(filename, _MAX_PATH, argv[FILENAME_RCVR]);
	port = strtol(argv[PORT_RCVR], NULL, 10);
	fopen_s(&message, filename, "wb");
	if (!simplestartup(&startup))
		return ERRORCODE;
	address = initaddress(LOCALHOST, port);
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET)
		exit(ERRORCODE);
	maxfd = s;
	if (bind(s, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
		printf("%d", 1);
	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(s, &fds);

		n = select(maxfd + 1, &fds, NULL, NULL, &waittime);

		if (_kbhit())
		{
			char c = _getch();
			if (c == 13)
			{
				printf("\n");
				if (!strcmp(endbuf, END))
					break;
				else
				{
					endbuf[3] = '\0';
					keystrokes = -1;
				}
			}
			else
			{
				if (c > 31 && c < 127)
				putc(c, stderr);
				if (keystrokes < strlen(END) + 1)
					endbuf[keystrokes] = c;				
			}
			keystrokes++;
		}
		while (n > 0)
		{
			if (FD_ISSET(s, &fds))
			{
				if (recvfrom(s, rcvbuf, sizeof(rcvbuf), 0, (SOCKADDR*)&address, &senderaddrsize) == SOCKET_ERROR)
				{
					printf("RECV ERROR %d\n", WSAGetLastError());
				}
				buffer[sll_i] = rcvbuf[0];
				buffer[sll_i + 1] = rcvbuf[1];
				sll_i += 2;
				if (sll_i > BUFFER_SIZE_RCVR - 2)
				{
					sll_i = 0;
					printf("1. buffer is: %s\n", buffer);
					logicshiftleftby11(buffer);
					//fputs(buffer, stderr);
				}
			}
			n--;
		}
	}
	fclose(message);
	return SUCCESSCODE;
}

//void logicshiftleftby11(string str)
//{
//	int temp, mask, goodbits = 11, badbits = 5;
//	int twobytes[8] = { 0 };
//	for (int i = 0; i < 16; i += 2)
//	{
//		twobytes[i / 2] = str[i] + (str[i + 1] << 8);
//	}
//	for (int i = 0; i < 4; i++)
//	{
//		mask = pow(2, min(badbits, 11)) - 1;
//		temp = twobytes[i + 1] & mask;
//		temp = temp << goodbits;
//		twobytes[i] |= temp;
//		twobytes[i + 1] = twobytes[i + 1] >> min(badbits, 11);
//		if (badbits > 11)
//		{
//			for (int j = i + 1; j < 7; j++)
//			{
//				twobytes[j] = twobytes[j + 1];
//			}
//			badbits -= 11;
//			goodbits += 11;
//			i--;
//		}
//		goodbits -= 5;
//		badbits += 5;
//	}
//	for (int i = 0; i < 8; i ++)
//	{
//		mask = 0xFF;
//		str[2 * i] = twobytes[i] & mask;
//		mask = 0xFF00;
//		str[2 * i + 1] = (twobytes[i] & mask) >> 8;
//	}
//	str[BUFFER_SIZE_SNDR - 1] = '\0';
//	printf("2. buffer is: %s\n", str);
//}
void logicshiftleftby11(string str)
{
	int64_t word1 = 0, word2 = 0, mask1 = 0xFF, mask2 = 0x7;
	int shift = 0;
	for (int i = 0; i < 8; i++)
	{
		word1 += ((int64_t)str[i] + 256) % 256 << shift;
		//printf("%d\n", str[i]);
		word2 += ((int64_t)str[i + 8] + 256) % 256 << shift;
		for (int j = 63; j >= 0; j--)
		{
			printf("%d", (word2 >> j) & 0x1);
			if (j % 4 == 0)
				printf(" ");
		}
		printf("%d\n", str[i + 8]);
		str[i] = '\0';
		shift += ((i % 2) ? 3 : 8);
	}
	word1 += (word2 & 0xF) << 44;
	word2 >>= 4;
	for (int i = 63; i >= 0; i--)
	{
		printf("%d", (word2 >> i) & 0x1);
		if (i % 4 == 0)
			printf(" ");
	}
	printf("\n");

	for (int i = 63; i >= 0; i--)
	{
		printf("%d", (word1 >> i) & 0x1);
		if (i % 4 == 0)
			printf(" ");
	}
	printf("\n");
	for (int i = 0; i < 6; i++)
	{
		str[i] = (word1 >> (i * 8)) & 0xFF;
	}
	for (int i = 0; i < 5; i++)
	{
		str[i + 6] = (word2 >> (i * 8)) & 0xFF;
	}
	printf("2. str: ");
	for (int i = 0; i < 16; i++)
	{
		if (str[i] == 0)
			printf("0'");
		else
			printf("%c", str[i]);
	}
	printf("\n");
	printf("str: ");
	for (int i = 0; i < 16; i++)
	{
		if (str[i] == 0)
			printf("0 ");
		else
			printf("%d ", str[i]);
	}
	printf("\n");

}