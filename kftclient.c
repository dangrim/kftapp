#include "kftclient.h"

int sock;                       /* Socket descriptor */
struct sockaddr_in servAddr; 	/* Server address */
struct sockaddr_in fromAddr;    /* Source address */
unsigned int fromSize;          /* In-out of address size for recvfrom() */
unsigned int servSize;          /* In-out of address size for recvfrom() */
int response_length;
u8 *in_buffer;      				/* Buffer for string */
u8 *out_buffer;      				/* Buffer for string */
struct sigaction myAction;      /* For setting signal handler */
char *local_file;						/* Local Filename */
unsigned int drop_percent;	/* Drop Percent */	

int debug = 0;
int tries = 0;
int offset = -1;
int prev_offset = -1;
u32 recv_msg_size = 0;
u16 max_packet_size = 0;

int main(int argc, char *argv[])
{
	struct timeval start, end;

	/* Arguments */
  u16 servPort;     					/* Server port */
  char *server;								/* Server IP */
	char *remote_file;					/* Remote Filename */

	int arg_i = 0;

	/* Check for Debug Mode */
	if(argc == 8 && ((strcmp(argv[1], "-d") == 0) || (strcmp(argv[1], "-D") == 0)))
	{
		debug = 1;
		arg_i++;
		printf("Entering debug mode\n.");
	}
	else if(argc != 7)
	{
		fprintf(stderr,"Usage: %s (-d || -D) <Server IP>  <Port #> <Remote File> <Local File> <Max Packet Size> <Drop Percent>\n", argv[0]);
	}
		
	/*ARGUMENTS & Checks*/
	server = argv[arg_i + 1];           						/* First arg:  server IP address (dotted quad) */
	servPort = atoi(argv[arg_i + 2]);   						/* Second arg: port # */
	printf("%d\n", servPort);
	if(servPort < 1024)
	{
		DieWithError("Your port number is too low.");
	}

  remote_file = argv[arg_i + 3];      						/* Third arg: Remote file name */
	if(strlen(remote_file) >997)
	{
		DieWithError("filename too large");
	}
	local_file = argv[arg_i + 4];										/* Fourth arg: Local file name */

	max_packet_size = (u16) atoi(argv[arg_i + 5]);	/* Fifth arg: Maximum Packet Size*/
  if (max_packet_size > 60000)
	{
    DieWithError("Max Packet Size incorrect");
		/*DieWithError("Please specify a packet size between %d and 60000\n");*/
	}

	drop_percent = atoi(argv[arg_i + 6]);						/* Sixth arg: Drop Rate*/
	if (drop_percent < 0 || drop_percent > 99)
	{
		DieWithError("Drop Percent must be between 1 and 99");
	}
	set_dropper(drop_percent);
	/* Show parsed arguments */	
	if(debug)
	{
	  printf("%s %d %s %s %u %d\n", server, servPort, remote_file, local_file, max_packet_size, drop_percent);
	}

	/* First Packet
	*	0 => (initiate connection)
	*	0-1 => max_packet_size
	* rest => filename
	*/

	out_buffer = pack_init_buffer(remote_file, strlen(remote_file), max_packet_size);

  /* Create a best-effort datagram socket using UDP */
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
    DieWithError("socket() failed");
	}

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

  /* Construct the server address structure */
  memset(&servAddr, 0, sizeof(servAddr));    /* Zero out structure */
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = inet_addr(server);  /* Server IP address */
  servAddr.sin_port = htons(servPort);       /* Server port */

	in_buffer = (u8 *) malloc(max_packet_size);
  /* Get a response */
	if(debug)
	{
		printf("Response Length: %d, Max Packet Size: %d", response_length, max_packet_size);
	}	
  fromSize = sizeof(fromAddr);
	servSize = sizeof(servAddr);
	gettimeofday(&start, NULL);
	while(1)
	{
		receive_and_send();		
		/*TO DO - FIX FIRST PACKET FAIL*/
		if(!unpack_int(in_buffer))
		{
			break;
		}
		make_request();		
		write_to_file();
		
		printf("Received: %s\n", in_buffer+8);    /* Print the received data */
	}
	gettimeofday(&end, NULL);

	int seconds = end.tv_sec - start.tv_sec;
	int microseconds = end.tv_usec - start.tv_usec;

	if(microseconds < 0)
	{
		seconds--;
		microseconds += 1000000;
	}

	if(debug)
	{
		printf("Transfer ended!\n");
	}
	printf("File transfer completed in %d seconds and %d microseconds.\n", seconds, microseconds);
	close(sock);
	exit(0);
}

void CatchAlarm(int ignored)
{
	tries++;
	if(debug)
	{
		printf("Failed Attempt #%d\n", tries);
	}}

/*
*	Prepared a buffer with an INITIAL REQUEST
*	1 byte - 0
*	2 bytes - max_packet_size
* remainder - filename
*/
char *pack_init_buffer(char *fname, int fname_length, u16 max_p_size)
{
	tries = 0;
	u8 *init_b = (u8 *)malloc(INITIAL_REQUEST_SIZE + fname_length);
	init_b[0] = 0;
	init_b[1] = max_p_size & 0xFF;
	init_b[2] = (max_p_size >> 8) & 0xFF;
	memcpy(init_b+INITIAL_REQUEST_SIZE, fname, fname_length);

	return init_b;
}

/*
*	Keeps writing a message until there is a response.
*/
int receive_and_send()
{
	write_msg();	
	ualarm(TIMEOUT_VALUE, 0);
	while((recv_msg_size = recvfrom(sock, in_buffer, max_packet_size, 0,
		(struct sockaddr *) &servAddr, &servSize)) < 0)
	{
		if(errno == EINTR)
		{
			write_msg();		
			ualarm(TIMEOUT_VALUE, 0);
		}
		else
		{
			return 1;
		}
	}
	ualarm(0, 0);
	return 0;
}

/*
*	creates a request that is sent to the server
*/
void make_request()
{
	int k = 0;
	out_buffer = (u8 *)malloc(REQUEST_SIZE);
	out_buffer[0] = 1;
	pack_int(out_buffer+1, unpack_int(in_buffer+4));
	
	if(debug)
	{
		printf("offset: %d\n", offset);
		for(k = 0; k < 5; k++)
		{
			printf("%d|", out_buffer[k]);
		}
		printf("%s\n", out_buffer+5);
	}
}
/*
*	Keep sending until it does not work. Upon success, increment offset.
*/
void write_msg()
{
	sendto_dropper(sock, out_buffer, max_packet_size, 0, (struct sockaddr *) &servAddr, sizeof(servAddr));
}

/*
*	puts an int into a char buffer
*/
void pack_int(u8 *buffer, u32 i)
{
	buffer[0] = i & 0xFF;
	buffer[1] = (i >> 8) & 0xFF;
	buffer[2] = (i >> 16) & 0xFF;
	buffer[3] = (i >> 24) & 0xFF;
}

/**
*	Write to the file if the offsets match.
*/
void write_to_file()
{
	u32 length = unpack_int(in_buffer);
	prev_offset = offset;
	offset = unpack_int(in_buffer+4);
	if(prev_offset < offset)
	{
		FILE *file;
		file = fopen (local_file, "ab");  /* open the file for reading */
		fwrite(in_buffer+8, 1, length, file);
		fclose(file);  /* close the file prior to exiting the routine */
	}
}

/*
*	takes an int from a char * pointer
*/
int unpack_int(u8 *buffer)
{
	int i = 0;
	i = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
	return i;
}

