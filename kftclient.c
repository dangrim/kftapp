#include "kftclient.h"

int debug = 0;
int sock;                       /* Socket descriptor */
struct sockaddr_in servAddr; 	/* Server address */
struct sockaddr_in fromAddr;    /* Source address */
unsigned int fromSize;          /* In-out of address size for recvfrom() */
int response_length;
char *buffer;      				/* Buffer for string */
struct sigaction myAction;      /* For setting signal handler */

int main(int argc, char *argv[])
{

	/* Arguments */
  u16 servPort;     					/* Server port */
  char *servIP;								/* Server IP */
	char *remote_file;					/* Remote Filename */
	char *local_file;						/* Local Filename */
  u16 max_packet_size;      	/* Length of string */
	unsigned int drop_percent;	/* Drop Percent */	


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
	servIP = argv[arg_i + 1];           						/* First arg:  server IP address (dotted quad) */
	servPort = atoi(argv[arg_i + 2]);   						/* Second arg: port # */
	printf("%d\n", servPort);
	if(servPort < 1024)
	{
		DieWithError("Your port number is too low.");
	}

  remote_file = argv[arg_i + 3];      						/* Third arg: Remote file name */
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

	/* Show parsed arguments */	
	if(debug)
	{
	  printf("%s %d %s %s %u %d\n", servIP, servPort, remote_file, local_file, max_packet_size, drop_percent);
	}

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
  servAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
  servAddr.sin_port = htons(servPort);       /* Server port */

  /* Send the Initial Request to the server */
  if ((response_length = sendto(sock, "TEST!", 5, 0, (struct sockaddr *)
             &servAddr, sizeof(servAddr))) < 0)
	{
	  DieWithError("Failed to Send initial request");
	}
	else
	{
		printf("Initial Request successful, %d bytes sent.\n", response_length);
	}
	
	buffer = (char *) malloc(max_packet_size);
  /* Get a response */
  if((response_length = recvfrom(sock, buffer, max_packet_size, 0,
         (struct sockaddr *) &fromAddr, &fromSize)) == max_packet_size)
  {

  }
	
	if(debug)
	{
		printf("Response Length: %d, Max Packet Size: %d", response_length, max_packet_size);
	}	
  
  fromSize = sizeof(fromAddr);
	printf("Received: %s\n", buffer);    /* Print the received data */
	close(sock);
	exit(0);
}

void CatchAlarm(int ignored)
{

}
