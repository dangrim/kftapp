#include "kftserver.h"

int debug = 0;
int acknowledged = 0;
int offset = 0;
int free_connection = 1;
u32 max_packet_size = 0;

int sock;                     /* Socket */
int recv_msg_size;
int send_msg_size;
char * filename;

struct sockaddr_in servAddr; 	/* Local address */
struct sockaddr_in clntAddr; 	/* Client address */
struct sigaction myAction;      /* For setting signal handler */
unsigned int cliAddrLen;      /* Length of incoming message */
char *in_buffer;        					/* Buffer for string */
char *out_buffer;
FILE *read_file;

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

		in_buffer = (char *)malloc(1000);		

    while(1) /* Run forever */
    {
      /* Set the size of the in-out parameter */
			
			if(acknowledged != 1)
			{
				accept_request();
			}
			else
			{
				make_pkt(); //put information into buffer
				send_until_success();
			}
			printf("looptime, %d.\n", acknowledged);
		}
    /* NOT REACHED */
	return 0;
}

/*
*	Incoming request. If it is the first time, need to initialize.
*/
void accept_request()
{
  cliAddrLen = sizeof(clntAddr);
	if ((recv_msg_size = recvfrom(sock, in_buffer, MAX_REQUEST_SIZE, 0,
		(struct sockaddr *) &clntAddr, &cliAddrLen)) > 0)
		{
			if(debug)
			{
				printf("Received: %d bytes.\n", recv_msg_size);
				printf("First byte: %x.\n", in_buffer[0]);
				printf("Free Connection: %d.\n", free_connection);
			}
			/*Logic for checking what was received*/
			if(in_buffer[0] == 0 && free_connection == 1)
			{
				printf("new connection, time to init\n");
				free_connection = 0;
				init_transfer(in_buffer, recv_msg_size);
				acknowledged = 1;
			}
			if(1 == in_buffer[0] && !free_connection){
				/*continue transaction*/
				acknowledged = 1;
			}
		}
}

/*
*	Keep sending until it does not work. Upon success, increment offset.
*/
void send_until_success()
{

	send_msg_size = sendto(sock, out_buffer, max_packet_size, 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr));

	alarm(5);
	while((recv_msg_size = recvfrom(sock, in_buffer, MAX_REQUEST_SIZE, 0,
		(struct sockaddr *) &clntAddr, &cliAddrLen)) < 0)
	{
		
	}
	alarm(0);
}

/*
*	Request received, initialize max_packet_size & filename
*/
void init_transfer(char *buffer, u32 msg_size)
{
	max_packet_size = (buffer[1] + (buffer[2] << 8));
	filename = (char *)malloc(msg_size-INITIAL_REQUEST_SIZE);
	memcpy(filename, buffer+3, msg_size-INITIAL_REQUEST_SIZE);

  /* Set signal handler for alarm signal */
  myAction.sa_handler = CatchAlarm;
  if (sigfillset(&myAction.sa_mask) < 0) /* block everything in handler */
	{
    DieWithError("sigfillset() failed");
	}
  myAction.sa_flags = 0;

  if (sigaction(SIGALRM, &myAction, 0) < 0)
	{
  	DieWithError("sigaction() failed for SIGALRM");
	}

	if(debug)
	{
		printf("filename: %s, packet size: %d\n", filename, max_packet_size);
	}
}

/*
*	puts an int into a char buffer
*/
void pack_int(char *buffer, u32 i)
{
	buffer[0] = i & 0xFF;
	buffer[1] = (i >> 8) & 0xFF;
	buffer[2] = (i >> 16) & 0xFF;
	buffer[3] = (i >> 24) & 0xFF;
}

/*
*		PACKET FORMAT
*		4 bytes - length of data
*		4 bytes - offset
* 	length bytes - data
*/
void make_pkt()
{
	if(debug)
	{
		printf("making packet\n");
	}
	out_buffer = (char *)malloc(max_packet_size);
	int length = read_a_file(filename, out_buffer+RESPONSE_HEADER_SIZE, max_packet_size-RESPONSE_HEADER_SIZE);
	pack_int(out_buffer, length);	
	pack_int(out_buffer+4, offset);
}

/*
*	Reads a file at a particular offset.
*/
int read_a_file(char *filename, char *buffer, u16 read_length)
{
  int result;
 	/* "rb" means open the file for reading bytes */
	read_file = fopen(filename, "rb");  /* open the file for reading */
	if(NULL == read_file)
	{
		fprintf(stderr, "FILE NOTE FOUND.\n", filename);
		acknowledged = 0;
		return 1;
	}
	fseek(read_file, offset, SEEK_SET);
	result = fread(buffer, 1, read_length, read_file);
	printf("Read: %s\n", buffer);
	return result;
}

void CatchAlarm(int ignored)
{

}
//        printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));
