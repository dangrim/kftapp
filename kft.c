#include "kft.h"

/**
*	Calculates the checksum for a given packet.
* The checksum is the sum of the src port, dst port, and length.
*	if there is any carryover, it is carried to the least significant bit.
* The last step is to take the one's complement of the number.
*/
u16 calc_checksum(pkt *packet)
{
	u16 carryover;
	u16 result;
	u16 newResult;
	carryover = result = newResult = 0;
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
	/*packet->checksum = ~(newResult + carryover);*/
	return ~(newResult + carryover);
}

void deliver_data(char * data)
{
	
}

pkt *extract(char *buffer, int buffer_length)
{
	pkt *new_packet = (pkt *)malloc(sizeof(pkt));
	new_packet->src_port = buffer[0] | (buffer[1] << 8);
	new_packet->dst_port = buffer[2] | (buffer[3] << 8);
	new_packet->length =  buffer[4] | (buffer[5] << 8);
	new_packet->checksum = buffer[6] | (buffer[7] << 8);
	new_packet->data = (char *) malloc(new_packet->length);
	char *data = (char *) (buffer[8] | (buffer[9] << 8) | (buffer[10] << 16) | (buffer[11] << 24));
	new_packet->data = (char *) memcpy(new_packet->data, data, new_packet->length);
	return new_packet;
}

/**
*	Create a packet
*/
pkt *make_pkt(char *data, int checksum)
{
	pkt *packet = malloc(sizeof(pkt));
	if(NULL != packet)
	{
		packet->data = data;
	}
	return packet;
}

/**
*	Create a request
*/
request *make_request(char *filename, int name_length, u16 max_packet_size, u8 ack)
{
	request *r = (request *) malloc(sizeof(request));
	if(NULL != r)
	{
		r->filename = filename;
		r->name_length = name_length;
		r->max_packet_size = max_packet_size;
		r->ack = ack;
	}
	return r;
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
	if(NULL != packet)
	{
		packet->src_port = 26208;
		packet->dst_port = 21845;
		packet->length = 36620;
		
		u16 correct = 19138;
		correct = ~correct & (0xFFFF);
		u16 result = calc_checksum(packet);
		printf("Checksum: %u\nCorrect: %u\n", result, correct);    /* Print the received data */
	}
}

void testPacket()
{
	printf("FAIL");
	pkt *packet;
	pkt *copy;
	
	packet = (pkt *) malloc(sizeof(pkt));
	if(NULL != packet){
		packet->src_port = 26208;
		packet->dst_port = 21845;
		packet->length = 5;

		packet->checksum = calc_checksum(packet);
		packet->data = "hello";

		copy = extract((char *)packet, (packet->length + PACKET_HEADER_SIZE + 4));

		printf("Copy Src: %u , Packet Src: %u\n", copy->src_port, packet->src_port);
		printf("Copy Dst: %u , Packet Dst: %u\n", copy->dst_port, packet->dst_port);
		printf("Copy Length: %u , Packet Length: %u\n", copy->length, packet->length);
		printf("Copy Checksum: %u , Packet Checksum: %u\n", copy->checksum, packet->checksum);
		printf("Copy Data: %s , Packet Data %s", copy->data, packet->data);
		if(copy->src_port == packet->src_port &&
			copy->dst_port == packet->dst_port &&
			copy->length == packet->length &&
			copy->checksum == packet->checksum)
		{
			printf("Both packets are equal.\n");
		}
		else
		{
			printf("Packets are not equal.\n");
		}
	}
}


void main()
{
	testPacket();
	/*testChecksum();*/
	/*int * buffer = malloc(sizeof(int));
	unsigned int b = (unsigned int) buffer;*/
}
