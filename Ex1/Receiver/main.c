

#include "StructsAndEnums.h"
#include "Defines.h"
#include <string.h>
#include "WSA.h"
#include <conio.h>
#include <math.h>
#include <inttypes.h>

void logicshiftleftby11(string str);
int nonblockingrecv();
void unHamming(string str);
BOOL ispowof2(int num);

int main(int argc, string* argv)
{
	assert(argc == NUM_OF_ARGC_RCVR);
	char buffer[BUFFER_SIZE_RCVR] = "\0", filename[_MAX_PATH] = { 0 }, rcvbuf[3] = { 0 }, endbuf[5] = "\0", rcvstr[321] = { 0 };
	int port = 0, maxfd, n;
	TIMEVAL waittime = { 0 };
	unsigned int keystrokes = 0, sll_i = 0, blocki = 0;
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

		select(maxfd + 1, &fds, NULL, NULL, &waittime);

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
		if (FD_ISSET(s, &fds))
		{
			blocki += recvfrom(s, rcvbuf, sizeof(rcvbuf) - 1, 0, (SOCKADDR*)&address, &senderaddrsize);
			//printf("blocki: %d, rcvstr: %s\n", blocki, rcvstr);
			if (blocki == SOCKET_ERROR)
			{
				printf("RECV ERROR %d\n", WSAGetLastError());
			}
			//strncpy_s(rcvbuf, sizeof(rcvbuf), rcvstr + i, sizeof(rcvbuf) - 1);
			//printf("rcvbuf before hamming: %s\n", rcvbuf);
			unHamming(rcvbuf);
			//printf("rcvbuf after hamming: %s\n", rcvbuf);
			buffer[sll_i] = rcvbuf[0];
			buffer[sll_i + 1] = rcvbuf[1];
			sll_i += 2;
			if (sll_i > BUFFER_SIZE_RCVR - 2)
			{
				sll_i = 0;
				logicshiftleftby11(buffer);
				//printf("buffer right before writing is: %s\n", buffer);
				fputs(buffer, message);
			}
			printf("%.2f%%\n", blocki / 298.888);
		}
	}
	printf("Done!\n");
	fclose(message);
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

void unHamming(string str)
{
	unsigned int unhamming = 0, databits[16] = { 0 }, checkbits[5] = { 0 }, j = 0;
	unhamming = (unsigned int)str[0] & 0xFF;
	unhamming += (((unsigned int)str[1]) << BYTESIZE) & 0xFF00;
	for (int i = 0; i < 16; i++)
	{
		databits[i] = ((unhamming >> i) & 0x1);
	}
	for (int i = 0; i < 4; i++)
	{
		checkbits[i + 1] = unhamming & (int)pow(2, pow(2, i));
	}

	// check errors

	str[0] = 0;
	str[1] = 0;
	str[2] = 0;
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
  }

//for (int i = 15; i > 0; i--)
//{
//	printf("%d", (unhamming >> i) & 0x1);
//	if (i % 4 == 0)
//		printf(" ");
//}
//printf("\n");