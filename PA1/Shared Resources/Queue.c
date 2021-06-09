#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Queue.h"
#include "Defines.h"
#include <string.h>

QClient* New__QClient(string packet,int packet_len)
{
	QClient* new_client = (QClient*)malloc(sizeof(QClient));
	if (new_client == NULL)
	{
		return NULL;
	}
	snprintf(new_client->packet, PACKETSIZE, "%s", packet);
	new_client->previous = NULL;
	new_client->packet_len = packet_len;
	return new_client;
}

Queue* New__Queue()
{
	Queue* new_Q = (Queue*)malloc(sizeof(Queue));
	if (new_Q == NULL)
	{
		return NULL;
	}
	new_Q->first = NULL;
	new_Q->last = NULL;
	new_Q->client_count = 0;
	return new_Q;
}

int Push__Queue(Queue* my_Q , string packet, int packet_len)
{
	QClient* new_QC = New__QClient(packet, packet_len);
	if (new_QC == NULL)
	{
		return ERRORCODE;
	}
	if (Empty__Queue(my_Q))
	{		
		my_Q->first = new_QC;
		my_Q->last = new_QC;
	}
	else
	{
		my_Q->last->previous = new_QC;
		my_Q->last = new_QC;
	}
	(my_Q->client_count)++;
	return 0;
}

string Pop__Queue(Queue* my_Q,int * get_len)
{
	if (Empty__Queue(my_Q))
	{
		return NULL;
	}
	else
	{
		char retval[PACKETSIZE];
		snprintf(retval, PACKETSIZE, "%s", my_Q->first->packet);
		*get_len = my_Q->first->packet_len;
		QClient* temp = my_Q->first;
		if (my_Q->first == my_Q->last)
			my_Q->last = NULL;
		my_Q->first = temp->previous;
		free(temp);
		(my_Q->client_count)--;
		return retval;
	}
}

string Top__Queue(Queue* my_Q)
{
	if (Empty__Queue(my_Q))
	{
		return -1;
	}
	else
	{				
		return my_Q->first->packet;
	}
}

bool Empty__Queue (Queue* my_Q)
{	
	if (my_Q->first == NULL || my_Q->last == NULL)
	{
		return true;
	}
	if (my_Q->client_count == 0)
	{
		return true;
	}
	return false;
}

void Destroy__Queue(Queue* my_Q)
{
	while (!Empty__Queue(my_Q))		
	{
		int NotUsed;
		Pop__Queue(my_Q, &NotUsed);
	}
	free(my_Q);
}
