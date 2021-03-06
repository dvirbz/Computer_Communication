

#include "StructsAndEnums.h"
#include "Defines.h"
#include <string.h>
#include "WSA.h"
#include <inttypes.h>
#include "IOMalloc.h"
#include <stdlib.h>
#include <time.h>
#include "IOMalloc.h"

int flip(int len_packet, int start, int end, double q, double b);
int flip_packet(int k, int recv_bytes_from_sender, char* packet, double q, double b);
void Handle_FinalMessage(SOCKET sd, SOCKADDR* address_of_rcvr,
	SOCKADDR* address_to_send_to_sndr, string ip_rcvr, int blocki, int tot_flips, string packet, int recv_bytes_from_sender);
void Calculate_RandomVariables(double prob, int* p_k, double* p_b, double* p_q, double* p_a);

int main(int argc, string* argv)
{
	assert(argc == NUM_OF_ARGC_CHNL);
/*	Variables definitions  */

	char packet[PACKETSIZE] = { 0 };
	int listen_to_sender = 0, maxfd, seed = 0, n_prob = 0, rcvr_listen_on = 0, tot_flips = 0,
		recv_bytes_from_sender = 0, recv_bytes_from_recv = 0, temp = -1, flips = 0, k = K_INIT + 1;
	double prob = 0, q, a, b = 2;
	TIMEVAL waittime = { .tv_sec = 0, .tv_usec = 1000 };
	unsigned int blocki = 0;
	fd_set read_fds, write_fds;
	WSADATA startup;
	SOCKADDR_IN address_of_rcvr, address_of_sndr, address_to_send_to_sndr = { 0 }, address_from_rcv = { 0 };
	SOCKET sd;
	int addrsize = sizeof(SOCKADDR_IN);

/*==============================================================================================================*/		
	address_to_send_to_sndr = Init__Address(0, 0);
	listen_to_sender = strtol(argv[PORT_CHNL], NULL, 10);
	rcvr_listen_on = strtol(argv[PORT_TO_RCVR], NULL, 10);
	seed = strtol(argv[SEED], NULL, 10);
	srand((unsigned int)seed);
	n_prob = strtol(argv[RND_CHANCE], NULL, 10);
	prob = n_prob / pow(2, 16);
	/*	Math to establish the random related variables  */
	Calculate_RandomVariables(prob, &k, &b, &q, &a);
	/*==========================================================================================*/

	/*Start WSA*/
	if (!Simple__Startup(&startup))
		return ERRORCODE;
	/*==========================================================================================*/

	/*Channel Address init and bind*/	
	address_of_sndr = Init__Address(LOCALHOST, listen_to_sender);
	address_of_rcvr = Init__Address(argv[IP_RCVR], rcvr_listen_on);
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Failed to Create Socket with Error Code %d\n", WSAGetLastError());
		exit(ERRORCODE);
	}
	if (bind(sd, (SOCKADDR*)&address_of_sndr, addrsize))
	{
		printf("Failed to Bind Socket with Error Code %d\n", WSAGetLastError());
	}
	/*==========================================================================================*/

	maxfd = sd;
	int count_packets = 0;

	while (1)
	{
		FD_ZERO(&read_fds);
		FD_SET(sd, &read_fds);
		FD_ZERO(&write_fds);
		FD_SET(sd, &write_fds);		

		select(maxfd + 1, &read_fds, &write_fds, NULL, &waittime);

		if (FD_ISSET(sd, &read_fds))// && FD_ISSET(send_to_rcvr,&write_fds))
		{
			if ((recv_bytes_from_sender = recvfrom(sd, packet, sizeof(packet) - 1, 0, (SOCKADDR*)&address_from_rcv, &addrsize)) == SOCKET_ERROR)
			{				
				printf("RECV ERROR %d\n", WSAGetLastError());				
			}
			if (address_to_send_to_sndr.sin_port == 0)
			{
				memcpy(&address_to_send_to_sndr, &address_from_rcv, sizeof(SOCKADDR));
			}

			if (!memcmp(&address_to_send_to_sndr, &address_from_rcv, sizeof(SOCKADDR)))
			{
				count_packets++;
				tot_flips += flip_packet(k, recv_bytes_from_sender, packet, q, b);
				blocki += recv_bytes_from_sender;
				MC__Send(packet, recv_bytes_from_sender, sd, (SOCKADDR*)&address_of_rcvr);
				
			}

			else
			{
				Handle_FinalMessage(sd, (SOCKADDR*)&address_of_rcvr,
					(SOCKADDR*)&address_to_send_to_sndr, argv[IP_RCVR], blocki, tot_flips, packet, recv_bytes_from_sender);
				break;
			}

			
		}
		int x = (blocki / 6116790.15) * 10;
		if (x != temp)
		{
			printf("%.2f%%\n", blocki / 6116790.15);
		}

		temp = x;
	}
	printf("Done!\n");
	closesocket(sd);
	return SUCCESSCODE;
}

int flip(int len_packet, int start, int end, double q, double b)
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
		return flip(len_packet, start, (start + end) / 2, pow(q, b), b) + flip(len_packet, ((start + end) / 2 + 1), end, pow(q, b), b);
	}
	return 0;
}

int flip_packet(int k, int recv_bytes_from_sender, char* packet, double q, double b)
{
	int flips = 0;
	int POW = (int)pow(2, (double)(K_INIT)-k);
	int recv_bits = recv_bytes_from_sender * 8;	
	for (int i = 0; i < POW; i++)
	{
		flips += flip(recv_bits, i * (int)pow(2, k), (i + 1) * (int)pow(2, k) - 1, q, b);
	}
	int* list_rnd = { 0 };
	if (!intalloc(&list_rnd, flips, C))
	{
		return ERRORCODE;
	}
	for (int i = 0; i < flips; i++)
	{
		int rnd = rand() % recv_bits;
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
	return flips;
}

void Handle_FinalMessage(SOCKET sd, SOCKADDR* address_of_rcvr,
	SOCKADDR* address_to_send_to_sndr , string ip_rcvr, int blocki, int tot_flips, string packet, int recv_bytes_from_sender)
{
	char ip[MAX_IP_LEN];
	int addrsize = sizeof(SOCKADDR);
	/*	Print Final message  */
	inet_ntop(AF_INET, &((SOCKADDR_IN*)address_to_send_to_sndr)->sin_addr, ip, sizeof(ip));
	printf("sender: %s\nreceiver: %s\n%d bytes, flipped %d bits\n", ip, ip_rcvr, blocki, tot_flips);
	
	/*  Send Final message to sender*/
	if (SOCKET_ERROR == sendto(sd, packet, recv_bytes_from_sender, 0, address_to_send_to_sndr, addrsize))
	{
		printf("Failed to Send Data Through Socket with Error Code %d\n", WSAGetLastError());
	}		
}

void Calculate_RandomVariables(double prob, int* p_k, double* p_b, double* p_q, double* p_a)
{
	double b = *p_b, q = *p_q, a = *p_a;
	int k = *p_k;
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
	} while (q == 1.0);
	*p_k = k, * p_b = b, * p_a = a, * p_q = q;
}
