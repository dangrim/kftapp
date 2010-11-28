struct Packet {
	
}pkt;


int corrupt(pkt *p);
void extract(pkt *p, char *data);
void handle_data(char *data);
int isACK(pkt *p);
int isNAK(pkt *p);
pkt make_pkt(char *data, int checksum);
pkt make_ack(int ack);
int rcv_pkt(pkt *p);
void send(char *data);
void send_pkt(pkt *p);
