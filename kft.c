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

pkt *extract(u8 *buffer)
{
  int i;
  char *d;
	pkt *new_packet = (pkt *)malloc(sizeof(pkt));
	
  /*Header Fields*/
	new_packet->src_port = buffer[0] | (buffer[1] << 8);
	new_packet->dst_port = buffer[2] | (buffer[3] << 8);
	new_packet->length =  buffer[4] | (buffer[5] << 8);
	new_packet->checksum = buffer[6] | (buffer[7] << 8);
	
	d = (char *)(buffer[8] + (buffer[9] << 8) + (buffer[10] << 16) + (buffer[11] << 24));
	
	new_packet->data = malloc(new_packet->length);
	if(NULL != new_packet->data)
	{
	  memcpy(new_packet->data, (char *) d, new_packet->length);
  }
	return new_packet;
}

request *extract_request(u8 *buffer)
{
  char *name;
  char *d;
	request *new_request = (request *)malloc(sizeof(request));
	
  /*Header Fields*/
	name = (char *)(buffer[8] + (buffer[1] << 8) + (buffer[2] << 16) + (buffer[3] << 24));
	new_request->max_packet_size =  buffer[4] | (buffer[5] << 8);
	new_request->ack = buffer[6];
	
	d = (char *)(buffer[8] + (buffer[9] << 8) + (buffer[10] << 16) + (buffer[11] << 24));
	
	new_request->filename = malloc(strlen(name));
	if(NULL != new_request->filename)
	{
	  memcpy(new_request->filename, name, strlen(name));
  }
	return new_request;
}

/**
*	Create a packet
*/
pkt *make_pkt(u8 *data)
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

/**
* Test to see if calc_checksum worked properly
*/
void testChecksum()
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
		if(correct == result){
		  printf("Checksum Test - SUCCESS\n");
	  }
		/*
		printf("Checksum: %u\nCorrect: %u\n", result, correct);    /* Print the received data */
	}
}

void testExtract()
{
	pkt *packet;
	pkt *copy;
	
	packet = (pkt *) malloc(sizeof(pkt));
	if(NULL != packet){
		packet->src_port = 26208;
		packet->dst_port = 21845;
		packet->length = 5;

		packet->checksum = calc_checksum(packet);
		packet->data = "hello";

		copy = extract((char *)packet);
    
    /*
		printf("Copy Src: %u , Packet Src: %u\n", copy->src_port, packet->src_port);
		printf("Copy Dst: %u , Packet Dst: %u\n", copy->dst_port, packet->dst_port);
		printf("Copy Length: %u , Packet Length: %u\n", copy->length, packet->length);
		printf("Copy Checksum: %u , Packet Checksum: %u\n", copy->checksum, packet->checksum);
		printf("Copy Data: %s, Packet Data: %s\n", copy->data, packet->data);
		*/
		
		if(copy->src_port == packet->src_port &&
			copy->dst_port == packet->dst_port &&
			copy->length == packet->length &&
			copy->checksum == packet->checksum &&
			(0 == strcmp(copy->data, packet->data)))
		{
			printf("Extract Test - SUCCESS.\n");
		}
		else
		{
			printf("Extract Test - FAIL.\n");
		}
	}
}


/*void main()
{
  testChecksum();
  testExtract();
}*/
