#include "kftserver.h"

int debug = 0;
int acknowledged = 0;
int sock;                     /* Socket */
int recv_msg_size;
struct sockaddr_in servAddr; 	/* Local address */
struct sockaddr_in clntAddr; 	/* Client address */
unsigned int cliAddrLen;      /* Length of incoming message */
char *buffer;        					/* Buffer for string */

int main(int argc, char *argv[])
{

		/*Arguments*/
    u16 servPort;     						/* 1st argument - server port */
		int drop_percent;							/* 2nd argument - drop percentage */		
		int arg_i = 0;		

		/* Check for Debug Mode */
		if(argc == 4 && ((strcmp(argv[1], "-d") == 0) || (strcmp(argv[1], "-D") == 0)))
		{
			debug = 1;
			arg_i++;
			printf("Entering debug mode.\n");
		}
		else if(argc != 3)
		{
        fprintf(stderr,"Usage:  %s (-d || -D) <UDP SERVER PORT> <Drop Percent>\n", argv[0]);
		}

		/* Port to listen to */
    servPort = atoi(argv[arg_i+1]);
		drop_percent = atoi(argv[arg_i+2]);

		/* Show parsed arguments */	
		if(debug)
		{
			printf("%u %d\n", servPort, drop_percent);
		}
		
		/* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&servAddr, 0, sizeof(servAddr));   		/* Zero out structure */
    servAddr.sin_family = AF_INET;                	/* Internet address family */
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); 	/* Any incoming interface */
    servAddr.sin_port = htons(servPort);      		/* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError("bind() failed");

    while(1) /* Run forever */
    {
      /* Set the size of the in-out parameter */
			
			if(!acknowledged)
			{
				accept_request();
			}
			else
			{
				make_pkt(); //put information into buffer
				send_until_success();
			}
		}
    /* NOT REACHED */
	return 0;
}

/**/
void accept_request()
{
  cliAddrLen = sizeof(clntAddr);
	if ((recv_msg_size = recvfrom(sock, buffer, MAX_REQUEST_SIZE, 0,
		(struct sockaddr *) &clntAddr, &cliAddrLen)) > 0)
		{
			if(debug)
			{
				printf("Received: %d bytes.\n", recv_msg_size);
		  	printf("Data: %s\n", buffer);
			}
			acknowledged = 1;
		}
}

void make_pkt()
{

}

void send_until_success()
{

}
//        printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));
