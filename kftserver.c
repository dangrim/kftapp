#include "kftserver.h"

int debug = 0;
int acknowledged = 0;
u32 offset = 0;
int free_connection = 1;
u32 max_packet_size = 0;
int tries = 0;
int finish = 0;
int resend = 0;
int drop_percent;							/* 2nd argument - drop percentage */		


int sock;                     /* Socket */
int recv_msg_size;
int send_msg_size;
char *filename;

struct sockaddr_in servAddr; 	/* Local address */
struct sockaddr_in clntAddr; 	/* Client address */
struct sigaction myAction;      /* For setting signal handler */
unsigned int cliAddrLen;      /* Length of incoming message */
u8 *in_buffer;        					/* Buffer for string */
u8 *out_buffer;
FILE *read_file;

int main(int argc, char *argv[])
{
		/*Arguments*/
    u16 servPort;     						/* 1st argument - server */
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
		if(servPort < 1024)
		{
			DieWithError("Your port number is too low.");
		}

		drop_percent = atoi(argv[arg_i+2]);
		if (drop_percent < 0 || drop_percent > 99)
		{
			DieWithError("Drop Percent must be between 1 and 99");
		}
		set_dropper(drop_percent);
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

		in_buffer = (u8 *)malloc(MAX_REQUEST_SIZE);		

    while(1) /* Run forever */
    {	
			if(acknowledged != 1)
			{
				accept_request();
			}
			else
			{
				make_pkt(); //put information into buffer
				if(send_until_success()) //try to send
				{
					printf("Um...huge fail");
				}
				assess();
			}
		}
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
			if(in_buffer[0] == 0 && (free_connection == 1))
			{
				printf("new connection, time to init\n");
				init_transfer(in_buffer, recv_msg_size);
			}
			else
			{
				acknowledged = 1;
			}
		}
}

/*
*	Keep sending until it does not work. Upon success, increment offset.
*/
int send_until_success()
{
	write_msg();	
	ualarm(TIMEOUT_VALUE, 0);
	while((recv_msg_size = recvfrom(sock, in_buffer, max_packet_size, 0,
		(struct sockaddr *) &clntAddr, &cliAddrLen)) < 0)
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
	if(debug)
	{
		printf("Msg Size: %d.\n", recv_msg_size);
	}
	return 0;
}

/*
*	Request received, initialize max_packet_size & filename
*/
void init_transfer(u8 *buffer, u32 msg_size)
{
	tries = 0;
	free_connection = 0;
	max_packet_size = (buffer[1] + (buffer[2] << 8));
	printf("Packet Size: %d, file_name %s, msg_size %d", max_packet_size, buffer+3, msg_size-3);
	filename = (char *)malloc(msg_size-INITIAL_REQUEST_SIZE);
	memcpy(filename, buffer+INITIAL_REQUEST_SIZE, msg_size-INITIAL_REQUEST_SIZE);

	in_buffer = (u8 *)malloc(max_packet_size);
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
	acknowledged = 1;
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

/*
*		PACKET FORMAT
*		4 bytes - length of data
*		4 bytes - offset
* 	length bytes - data
*/
void make_pkt()
{
	out_buffer = (u8 *)malloc(max_packet_size);
	int length = read_a_file(filename, out_buffer+RESPONSE_HEADER_SIZE, max_packet_size-RESPONSE_HEADER_SIZE);
	int k;
	if(length == 0)
	{
		free_connection = 1;
		acknowledged = 0;
		finish = 1;
		if(debug)
		{
			printf("No More Data\n");
		}
	}
	pack_int(out_buffer, length);	
	pack_int(out_buffer+4, offset);
	if(debug)
	{
		//printf("Packet Length: %d, Offset: %d.\n", length, offset);
		for(k = 0; k < 8; k++)
		{
			printf("%u|", out_buffer[k]);
		}		
		printf("%s\n", out_buffer+8);
	}
}

/*
*	Reads a file at a particular offset.
*/
int read_a_file(char *filename, u8 *buffer, u16 read_length)
{
  int result;
 	/* "rb" means open the file for reading bytes */
	read_file = fopen(filename, "rb");  /* open the file for reading */
	if(NULL == read_file)
	{
		fprintf(stderr, "%s NOT FOUND.\n", filename);
		acknowledged = 0;
		free_connection = 0;
		return 1;
	}

	/*Seek the right spot in the file and read the information*/
	fseek(read_file, offset, SEEK_SET);
	result = fread(buffer, 1, read_length, read_file);
	fclose(read_file);

	if(result != read_length)
	{
		
		if(debug)
		{
			printf("File complete!\n");
		}
	}
	printf("Read: %s\n", buffer);
	return result;
}


/* checks out the state of the server and increments values accordingly*/
void assess()
{
	u32 off = unpack_int(in_buffer+1);
	free(out_buffer);
	if(debug)
	{
		printf("Offset %d, msg_offset %d\n", offset ,off);
	}
	if(off == offset && in_buffer[0])
	{
		if(debug)
		{
			printf("Offset incrementing");
		}
		offset += recv_msg_size-RESPONSE_HEADER_SIZE;	
	}
	else if(off != offset && in_buffer[0])
	{
		//acknowledged = 0;
	}
	else{
		init_transfer(in_buffer, recv_msg_size);
	}
}

/*sendto_dropper*/
void write_msg()
{
	sendto_dropper(sock, out_buffer, max_packet_size, 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr));
}

void CatchAlarm(int ignored)
{
	tries++;
}

/*removes an int from a char buffer*/
int unpack_int(u8 *buffer)
{
	int i = 0;
	i = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
	return i;
}
