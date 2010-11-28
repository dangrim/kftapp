typedef unsigned short u16;

typedef struct Packet {
	u16 src_port;
	u16 dst_port;
	u16 length;
	u16 checksum;
	char *data;
}pkt;

u16 calc_checksum(pkt *packet);
void deliver_data(char * data);
void extract(pkt *packet);
pkt *make_pkt(char *data, int checksum);
int rcv_pkt(pkt *p);
void send_data(char *data, int size);
void send_pkt(pkt *p);
