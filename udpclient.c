#include "udpclient.h"

int tries=0;   /* Count of times sent - GLOBAL for signal-handler access */

int main(int argc, char *argv[])
{
  int sock;                       /* Socket descriptor */
  struct sockaddr_in servAddr; 	/* Server address */
  struct sockaddr_in fromAddr;    /* Source address */

  unsigned int fromSize;          /* In-out of address size for recvfrom() */
  struct sigaction myAction;      /* For setting signal handler */

	char *buffer;      							/* Buffer for string */
	/*Debug*/
	int rcvCount, sendCount = 0;
	unsigned int drop_percent;
	int i = 0;

  u16 servPort;     	/* Server port */
  char *servIP;
	char *remote_file;
	char *local_file;
  u16 max_packet_size;      /* Length of string */
  int response_length = 0;         		/* Size of received datagram */
	request *initial;

	if (argc < 6 || argc > 7)    /* Test for correct number of arguments */
  {
      fprintf(stderr,"Usage: %s <Server IP>  <Port #> <Remote File> <Local File> <Max Packet Size> <Drop Percent>\n", argv[0]);
  }
	else if(argc == 7 && (strcmp(argv[1], "-d") != 0) )
	{
					
			i = 1;
      fprintf(stderr,"Usage: %s <Server IP>  <Port #> <Remote File> <Local File> <Max Packet Size> <Drop Percent>\n", argv[0]);		
	}
	
	/*ARGUMENTS & Checks*/
	servIP = argv[i+1];           /* First arg:  server IP address (dotted quad) */
	servPort = atoi(argv[i+2]);   /* Second arg: port # */
  remote_file = argv[i+3];      /* Third arg: Remote file name */
	local_file = argv[i+4];				/* Fourth arg: Local file name */
	max_packet_size = (u16) atoi(argv[i+5]);	/* FIfth arg: Maximum Packet Size*/
	drop_percent = atoi(argv[i+6];
	
  printf("%s %d %s %s %u\n", servIP, servPort, remote_file, local_file, max_packet_size);
  if (max_packet_size > 60000 || max_packet_size < PACKET_HEADER_SIZE+1)
	{
    DieWithError("Please specify a packet size between 1 and 60000");
	}

	/*allocate space for the biggest possible packet.*/
	buffer = (char *)malloc(sizeof(char) *max_packet_size);
	if(!buffer)
	{
		DieWithError("Unable to allocate memory for packets that large.");
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


	/*Make Initial Request*/
	initial = make_request(remote_file, strlen(remote_file), max_packet_size, ACK);

  /* Send the Initial Request to the server */
  if (sendto(sock, initial, sizeof(request), 0, (struct sockaddr *)
             &servAddr, sizeof(servAddr)) != sizeof(request))
	{
	  DieWithError("Failed to Send initial request");
	}
	else
	{
		printf("Initial Request successful.\n");
	}

  /* Get a response */
  if((response_length = recvfrom(sock, buffer, max_packet_size, 0,
         (struct sockaddr *) &fromAddr, &fromSize)) == sizeof(request) &&
         buffer[response_length-1] == ACK)
  {
    printf("Request granted.\n");
  }
  
  fromSize = sizeof(fromAddr);
  alarm(TIMEOUT_SECS);        /* Set the timeout */
	
	/*Receive message*/
  while ((response_length = recvfrom(sock, buffer, max_packet_size, 0,
         (struct sockaddr *) &fromAddr, &fromSize)) < 0)
	{
		pkt * packet = extract(buffer);
		
		/*Write message*/
		/*TODO: ADD NACK KNOWLEDGE*/
		write(local_file, packet->data, packet->length);

		/*Send ACK or NACK*/
		request *req = make_request(remote_file, strlen(remote_file), max_packet_size, ACK);
    if (sendto(sock, req, sizeof(request), 0, (struct sockaddr *)
          &servAddr, sizeof(servAddr)) != sizeof(request))
		{
          DieWithError("sendto() failed");
		}
		else
		{
			sendCount++;
			printf("Send Count: %d\n", sendCount);
		}
/*
    if (errno == EINTR)     /* Alarm went off  
    {
      if (tries < MAXTRIES)      /* incremented by signal handler 
      {
        printf("timed out, %d more tries...\n", MAXTRIES-tries);

        if (sendto(sock, req, sizeof(request), 0, (struct sockaddr *)
	            &servAddr, sizeof(servAddr)) != sizeof(request))
				{
              DieWithError("sendto() failed");
				}
				else
				{
					sendCount++;
					printf("Send Count: %d\n", sendCount);
				}
        alarm(TIMEOUT_SECS);
      } 
      else
			{
          DieWithError("No Response");
			}
  	} 
  	else
		{
      DieWithError("recvfrom() failed");
		}*/
		/*alarm(0);*/
		/* recvfrom() got something --  cancel the timeout */


		printf("Received: %s\n", packet->data);    /* Print the received data */
		}
	close(sock);
	exit(0);
}

void CatchAlarm(int ignored)     /* Handler for SIGALRM */
{
    tries += 1;
}
