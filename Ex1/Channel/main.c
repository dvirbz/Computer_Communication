

#include "StructsAndEnums.h"
#include "Defines.h"
#include <string.h>
#include "WSA.h"
#include <inttypes.h>
#include "IOMalloc.h"
#include <stdlib.h>
#include <time.h>
#include "IOMalloc.h"
int flip(string packet, int len_packet, int start, int end, double q, double b);


int main(int argc, string* argv)
{
	assert(argc == NUM_OF_ARGC_CHNL);
	char packet[PACKETSIZE] = { 0 };
	int port_channel = 0, maxfd, n, seed = 0, n_prob = 0, port_to_send_on = 0, tot_flips = 0,
		recv_bytes_from_sender = 0, recv_bytes_from_recv = 0, temp = -1, flips = 0, k = K_INIT + 1;
	double prob = 0, q, a, b = 2;
	TIMEVAL waittime = { 0 };
	unsigned int blocki = 0;
	fd_set read_fds, write_fds;
	WSADATA startup;
	SOCKADDR_IN address_rcvr, address_sndr;
	SOCKET send, recv, sender;
	int addrsize = sizeof(SOCKADDR_IN);
	waittime.tv_usec = 1000;
	port_channel = strtol(argv[PORT_CHNL], NULL, 10);
	port_to_send_on = strtol(argv[PORT_TO_RCVR], NULL, 10);
	seed = strtol(argv[SEED], NULL, 10);
	srand((unsigned int)seed);
	n_prob = strtol(argv[RND_CHANCE], NULL, 10);
	prob = n_prob / pow(2, 16);
	do
	{
		k--;
		b = 2;
		q = 1 - pow((1 - prob), pow(2, k));
		a = log(prob) / log(q);
		for (int i = 0; i < 100; i++)
		{
			b = pow(b * a - a + 1, 1 / (double)k);
		}
		printf("a is: %f\n", a);
		printf("b is: %f\n", b);
		printf("q is: %f\n", q);
		printf("p is: %f\n", prob);
		printf("k is: %d\n", k);
		Sleep(1000);
	} while (q == 1.0);


	if (!simplestartup(&startup))
		return ERRORCODE;

	/*Channel Address init*/	
	address_sndr = initaddress(LOCALHOST, port_channel);
	send = socket(AF_INET, SOCK_DGRAM, 0);
	if (send == INVALID_SOCKET)
		exit(ERRORCODE);

	/* Receiver Address init*/
	address_rcvr = initaddress(argv[IP_RCVR], port_to_send_on);
	recv = socket(AF_INET, SOCK_DGRAM, 0);
	if (recv == INVALID_SOCKET)
		exit(ERRORCODE);

	maxfd = max(send,recv);
	if (bind(send, (SOCKADDR*)&address_sndr, addrsize))
	{
		printf("Failed to Bind Socket with Error Code %d\n", WSAGetLastError());
	}
	int counter = 0;
	while (1)
	{
		FD_ZERO(&read_fds);
		FD_SET(send, &read_fds);
		FD_SET(recv, &read_fds);
		FD_ZERO(&write_fds);
		FD_SET(recv, &write_fds);
		//FD_SET(send, &write_fds);

		select(maxfd + 1, &read_fds, &write_fds, NULL, &waittime);

		if (FD_ISSET(recv, &read_fds))
		{
			char ip[MAX_IP_LEN];
			address_rcvr.sin_port = 0;
			closesocket(recv);

			if ((recv = socket(AF_INET, SOCK_DGRAM, 0) == SOCKET_ERROR))
			{
				printf("Failed to Create Socket with Error Code %d\n", WSAGetLastError());
			}

			if (bind(recv, (SOCKADDR*)&address_rcvr, addrsize))
			{
				printf("Failed to Bind Socket with Error Code %d\n", WSAGetLastError());
			}

			if ((recv_bytes_from_sender = recvfrom(recv, packet, sizeof(packet) - 1, 0, (SOCKADDR*)&address_rcvr, &addrsize)) == SOCKET_ERROR)
			{
				printf("Failed to Recv Data Through Socket with Error Code %d\n", WSAGetLastError());
			}

			inet_ntop(AF_INET,  &(address_sndr.sin_addr), ip, sizeof(ip));
			printf("sender: %s\nreceived: %s\n%d bytes, flipped %d bits\n", ip, argv[IP_RCVR], blocki, tot_flips);

			if ((sender = socket(AF_INET, SOCK_DGRAM, 0) == SOCKET_ERROR))
			{
				printf("Failed to Create Socket with Error Code %d\n", WSAGetLastError());
			}

			if (SOCKET_ERROR == sendto(sender, packet, recv_bytes_from_sender, 0, (SOCKADDR*)&address_sndr, addrsize))
			{
				printf("Failed to Send Data Through Socket with Error Code %d\n", WSAGetLastError());
			}
			Sleep(1000);
			closesocket(sender);
			break;
		}
		if (FD_ISSET(send, &read_fds) && FD_ISSET(recv,&write_fds))
		{
			recv_bytes_from_sender = recvfrom(send, packet, sizeof(packet) - 1, 0, (SOCKADDR*)&address_sndr, &addrsize);
			if (recv_bytes_from_sender == SOCKET_ERROR)
			{
				printf("RECV ERROR %d\n", WSAGetLastError());
			}
			counter++;
			int POW = pow(2, (double)(K_INIT) - k);
			for (int i = 0; i < POW; i++)
			{
				flips += flip(packet, recv_bytes_from_sender * 8, i * (int)pow(2, k), (i + 1) * (int)pow(2, k) - 1, q, b);
			}
			int* list_rnd = { 0 };
			if (!intalloc(&list_rnd, flips, C))
			{
				return ERRORCODE;
			}
			for (int i = 0; i < flips; i++)
			{
				int rnd = rand() % (recv_bytes_from_sender * 8);
				list_rnd[i] = rnd;
				int j = 0;
				for (j = 0; j < i; j++)
				{
					if (list_rnd[j] == rnd)
					{
						i--;
						j = -1;
						break;
					}
				}
				if (j == -1)
					continue;
				int byte = rnd / 8;
				int bit = rnd % 8;
				packet[byte] ^= (int)pow(2, bit);				
			}
			tot_flips += flips;
			flips = 0;
			sendto(recv, packet, recv_bytes_from_sender, 0, (SOCKADDR*)&address_rcvr, addrsize);			
			blocki += recv_bytes_from_sender;
			int x = (blocki / 6116790.15) * 10;
			if (x != temp)
			{
				printf("%.2f%%\n", blocki / 6116790.15);
				printf("flipped: %d ompared to %d packets\n", tot_flips, counter);
			}
			temp = x;
		}
	}
	printf("Done!\n");
	closesocket(send);
	closesocket(recv);
	return SUCCESSCODE;
}

int flip(string packet, int len_packet, int start, int end, double q, double b)
{
	if (start == end)
	{
		if (start < len_packet)
		{
			return 1;
		}
		return 0;
	}
	int temp = rand();		
	if (temp < q * pow(2, 15))
	{
		return flip(packet, len_packet, start, (start + end) / 2, pow(q, b), b) + flip(packet, len_packet, ((start + end) / 2 + 1), end, pow(q, b), b);
	}
	return 0;
}