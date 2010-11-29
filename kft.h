typedef unsigned short u16;
typedef unsigned char u8;

typedef struct Packet {
	u16 src_port;
	u16 dst_port;
	u16 length;
	u16 checksum;
	u8 *data;
}pkt;

typedef struct Request {
	char *filename;
	u16 max_packet_size;
	u8 ack;
}request;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PACKET_HEADER_SIZE  12
#define ACK   1
#define NACK  0

u16 calc_checksum(pkt *packet);
void deliver_data(char * data);
pkt *extract(u8 *buffer);
pkt *make_pkt(u8 *data);
request *make_request(char *filename, int name_length, u16 packet_size, u8 ack);
int rcv_pkt(pkt *p);
void send_data(char *data, int size);
void send_pkt(pkt *p);
