

#include "StructsAndEnums.h"
#include "Defines.h"
#include <string.h>
#include "WSA.h"
#include <inttypes.h>
#include "IOMalloc.h"
#include <stdlib.h>

void logicshiftrightby11(string str);
void hamming(string str);
void Add__Data(string p, string data, int* currentbyte);
void Get__Data(string p, string data, int* currentbyte);


int main(int argc, string* argv)
{
	assert(argc == NUM_OF_ARGC_CHNL);
	char packet[PACKETSIZE] = { 0 };
	int port_channel = 0, maxfd, n, seed = 0, n_prob = 0, port_to_send_on= 0, recv_bytes_from_sender = 0, recv_bytes_from_recv = 0, temp = -1;
	TIMEVAL waittime = { 0 };
	unsigned int blocki = 0;
	fd_set read_fds, write_fds;
	WSADATA startup;
	SOCKADDR_IN address_rcvr, address_sndr;
	SOCKET send, recv;
	int addrsize = sizeof(SOCKADDR_IN);
	waittime.tv_usec = 1000;
	port_channel = strtol(argv[PORT_CHNL], NULL, 10);
	port_to_send_on = strtol(argv[PORT_TO_RCVR], NULL, 10);
	seed = strtol(argv[SEED], NULL, 10);
	srand((unsigned int)seed);
	n_prob = strtol(argv[RND_CHANCE], NULL, 10);

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
	if (bind(send, (SOCKADDR*)&address_sndr, addrsize) == SOCKET_ERROR)
		printf("%d", 1);

	while (1)
	{
		FD_ZERO(&read_fds);
		FD_SET(send, &read_fds);
		FD_SET(recv, &read_fds);
		/*FD_ZERO(&write_fds);
		FD_SET(send, &write_fds);
		FD_SET(recv, &write_fds);*/

		select(maxfd + 1, &read_fds, NULL, NULL, &waittime);

		if (FD_ISSET(send, &read_fds))
		{
			recv_bytes_from_sender = recvfrom(send, packet, sizeof(packet) - 1, 0, (SOCKADDR*)&address_sndr, &addrsize);
			if (recv_bytes_from_sender == SOCKET_ERROR)
			{
				printf("RECV ERROR %d\n", WSAGetLastError());
			}
			sendto(recv, packet, recv_bytes_from_sender, 0, (SOCKADDR*)&address_rcvr, &addrsize);
			blocki += recv_bytes_from_sender;
			int x = (blocki / 6116790.15) * 10;
			if (x != temp)
				printf("%.2f%%\n", blocki / 6116790.15);
			temp = x;
		}
	}
	printf("Done!\n");
	closesocket(send);
	return SUCCESSCODE;
}