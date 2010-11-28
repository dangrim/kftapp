
/*
*	Will calculate checksum and see if there is a match
*	returns 1 if legit, else 0.
*/
int corrupt(pkt *p);

/*
*	Take the data from the packet and store it at data.
*/
void extract(pkt *p, char *data);

/*
*	Append the data to a file
*/
void handle_data(char *data);

/*
*	Returns 1 if the packet is an ACK
*/
int isACK(pkt *p);

/*
*	Returns 0 if the packet is an NAK
*/
int isNAK(pkt *p);

/*
*	Create a packet with data and checksum.
*/
pkt make_pkt(char *data, int checksum);

/*
*	Create an ACK if ack = 1, else NAk
*/
pkt make_ack(int ack);

/*
*	Receive a packet
*/
int rcv_pkt(pkt *p);

/*
*	Send data to the client.
*/
void send(char *data);

/*
*	Send a packet to the client
*/
void send_pkt(pkt *p);
