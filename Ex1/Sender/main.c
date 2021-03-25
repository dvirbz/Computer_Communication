

#include "StructsAndEnums.h"
#include "Defines.h"
#include <string.h>
#include "WSA.h"
#include <inttypes.h>
#include "IOMalloc.h"

FILE* Init(string ip, int* port, string* argv);
void logicshiftrightby11(string str);
void hamming(string str);
void Concat_Hamming(string buffer);
void Add_Data(string p, string data, int* currentbyte);
void Reset_String(string packet);
void Build_Packet(string buffer, string packet, SOCKET s, int* currentbyte, SOCKADDR* address, FILE* message);
void Send_Packet(SOCKET s, string packet, int* currentbyte, SOCKADDR* address, FILE* message);
void Final_Recv(SOCKET sr, SOCKADDR* address);


int main(int argc, string* argv)
{
	assert(argc == NUM_OF_ARGC_SNDR);
	char buffer[BUFFER_SIZE_RCVR] = { 0 }, ip[MAX_IP_LEN] = { 0 }, packet[PACKETSIZE] = { 0 };
	int port = 0, currentbyte = 0, maxfd, read;
	FILE* message = { 0 };
	SOCKADDR_IN address;
	SOCKET sd_write;
	fd_set fds_read, fds_write;
	Summary sum = { 0, 0, 0 };
	TIMEVAL waittime = { .tv_sec = 0, .tv_usec = 1000 };
	// Init File and Command Line Arguments // 
	if (NULL == (message = Init(ip, &port, argv)))
		return ERRORCODE;

	// Init Send and Receive addresses // 
	address = Init__Address(ip, port);
	
	// Init Sockets // 
	if (INVALID_SOCKET == (sd_write = socket(AF_INET, SOCK_DGRAM, 0)))
	{
		printf("Failed To Open Sockets, Error Code is: %d\n", WSAGetLastError());
		exit(ERRORCODE);
	}
	maxfd = sd_write;

	read = fread(buffer, 1, BUFFER_SIZE_SNDR - 1, message);
	if (read < BUFFER_SIZE_SNDR - 1)
	{
		printf("File is not Divisible by 11, Tail of %d Bytes left\n", read);
	}

	while (1)
	{
		FD_ZERO(&fds_read);
		FD_SET(sd_write, &fds_read);
		FD_ZERO(&fds_write);
		FD_SET(sd_write, &fds_write);
		select(maxfd + 1, &fds_read, &fds_write, NULL, &waittime);

		if (FD_ISSET(sd_write, &fds_read))
		{
			printf("%d\n", 1);
			Final_Recv(sd_write, (SOCKADDR*)&address);
			break;
		}

		if (!feof(message) && FD_ISSET(sd_write, &fds_write))
		{
			Build_Packet(buffer, packet, sd_write, &currentbyte, (SOCKADDR*)&address, message);
		}
	}

	fclose(message);
	if (closesocket(sd_write))
		printf("Failed To Close Socket\n");
	return SUCCESSCODE;
}

FILE* Init(string ip, int* port, string* argv)
{
	FILE* message;
	char filename[_MAX_PATH] = { 0 };
	WSADATA startup;
	strcpy_s(ip, MAX_IP_LEN, argv[IP_SNDR]);
	strcpy_s(filename, _MAX_PATH, argv[FILENAME_SNDR]);
	*port = strtol(argv[PORT_SNDR], NULL, 10);
	fopen_s(&message, filename, "rb");
	if (!Simple__Startup(&startup))
		return NULL;
	return message;
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
		word1 += ((unsigned long long)(str[i] + 256) % 256) << (BYTESIZE * i);
		word2 += ((unsigned long long)(str[i + DATASIZE / 2] + 256) % 256) << (BYTESIZE * i);
	}
	word2 = word2 << 16;
	word2 = word2 >> 20;
	for (int i = 0; i < 4; i++)
	{
		unsigned long long temp = (word1 >> (DATASIZE * i)) & mask1;
		str[2 * i] = temp;
		temp = (word1 >> (DATASIZE * i + BYTESIZE)) & mask2;
		str[2 * i + 1] = temp;
		temp = (word2 >> (DATASIZE * i)) & mask1;
		str[2 * i + BUFFER_SIZE_RCVR / 2] = temp;
		temp = (word2 >> (DATASIZE * i + BYTESIZE)) & mask2;
		str[2 * i + BUFFER_SIZE_RCVR / 2 + 1] = temp;
	}
}

void Add_Data(string p, string data, int* currentbyte)
{
	unsigned long long word1 = 0, word2 = 0, cmask, transfermask, temp = 0, compliment = 0;
	unsigned int byte_transfer;
	cmask = (unsigned long long)(pow(2, BYTESIZE) - 1);
	byte_transfer = 4 + BYTESIZE;
	transfermask = (unsigned long long)(pow(2, byte_transfer) - 1);
	for (int i = 0; i < 8; i += 2)
	{
		temp = (unsigned long long)((((unsigned long long)(data[i]) >> 1) & 0x7F)) + (unsigned long long)(((unsigned long long)(data[i + 1]) << (BYTESIZE - 1)));
		word1 += ((temp & 0x7FFF) << (HAMMINGSIZE * (i / 2)));
		temp = (unsigned long long)(((unsigned long long)(data[i + 8]) >> 1) & 0x7F) + (unsigned long long)((unsigned long long)(data[i + 9]) << (BYTESIZE - 1));
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
		if (i < 7)
			p[*currentbyte + i + 8] = (char)((word2 >> (BYTESIZE * i)) & 0xFF);
	}
	(*currentbyte) += 14;
}

void Concat_Hamming(string buffer)
{
	for (int i = 0; i < BUFFER_SIZE_RCVR - 1; i += 2)
	{
		hamming(buffer + i);
	}
	buffer[BUFFER_SIZE_RCVR - 1] = '\0';

}

void Reset_String(string packet)
{
	for (int i = 0; i < sizeof(packet) - 1; i++)
	{
		packet[i] = '\0';
	}
}

void Build_Packet(string buffer, string packet, SOCKET s, int* currentbyte, SOCKADDR* address, FILE* message)
{
	int read = 0;
	logicshiftrightby11(buffer);
	Concat_Hamming(buffer);
	Add_Data(packet, buffer, currentbyte);
	Send_Packet(s, packet, currentbyte, address, message);
	Reset_String(buffer);
	read = fread(buffer, 1, BUFFER_SIZE_SNDR - 1, message);
	if (read > 0 && read < BUFFER_SIZE_SNDR - 1)
	{
		printf("File is not Divisible by 11, Tail of %d Bytes left\n", read);
	}
	Send_Packet(s, packet, currentbyte, address, message);
}

void Send_Packet(SOCKET s, string packet, int *currentbyte, SOCKADDR* address, FILE* message)
{
	if ((feof(message) && *currentbyte != 0) || *currentbyte == PACKETSIZE - 1)
	{
		MC__Send(packet, *currentbyte, s, address);
		*currentbyte = 0;
		Reset_String(packet);
		Sleep(3);
	}
}

void Final_Recv(SOCKET s, SOCKADDR* address)
{
	char packet[PACKETSIZE] = { 0 };
	string temp, NotUsed = { 0 };
	int received, written, errors, addrsize = sizeof(SOCKADDR);
	if (SOCKET_ERROR == (recvfrom(s, packet, sizeof(packet) - 1, 0, address, &addrsize)))
	{
		printf("RECV ERROR %d\n", WSAGetLastError());
	}
	temp = strtok_s(packet, ",\r", &NotUsed);
	received = strtol(temp, NULL, 10);
	temp = strtok_s(NULL, ",\r", &NotUsed);
	written = strtol(temp, NULL, 10);
	temp = strtok_s(NULL, ",\r", &NotUsed);
	errors = strtol(temp, NULL, 10);
	printf("receiver: %d\nwritten: %d\ndetected & corrected: %d errors\n", received, written, errors);
}

