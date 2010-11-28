#include "kft.h"
#include <stdio.h>
#include <stdlib.h>

/**
*	Calculates the checksum for a given packet.
* The checksum is the sum of the src port, dst port, and length.
*	if there is any carryover, it is carried to the least significant bit.
* The last step is to take the one's complement of the number.
*/
u16 calc_checksum(pkt *packet)
{
	u16 carryover, result, newResult = 0;
	result = packet->src_port + packet->dst_port;

	if(result < packet->src_port || result < packet->dst_port)
	{
		carryover++;
	}

	newResult = packet->length + result;
	
	if(newResult < packet->length || newResult < result)
	{
		carryover++;
	}
	packet->checksum = ~(newResult + carryover);
	return ~(newResult + carryover);
}

void extract(pkt *packet)
{

}

void deliver_data(char * data)
{
	
}

/**
*	Create a packet
*/
pkt *make_pkt(char *data, int checksum)
{
	pkt *packet = malloc(sizeof(pkt));
	if(packet)
	{
		packet->data = data;
	}
	return packet;
}

int rcv_pkt(pkt *p)
{
	
}

void send_data(char *data, int size)
{

}

void send_pkt(pkt *p)
{

}

int testChecksum()
{
	pkt *packet;
	packet = (pkt *) malloc(sizeof(pkt));
	if(packet){
		packet->src_port = 26208;
		packet->dst_port = 21845;
		packet->length = 36620;
		
		u16 correct = 19138;
		correct = ~correct & (0xFFFF);
		u16 result = calc_checksum(packet);
		printf("Checksum: %u\nCorrect: %u\nPacket: %u\n", result, correct, packet->checksum);    /* Print the received data */
		if(result == correct)
		{
			printf("Checksum test was successful\n");
		}
		else{
			printf("Checksum test failed\n");
		}
	}
}

void main()
{
	testChecksum();
}
