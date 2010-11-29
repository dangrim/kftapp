#include "udpserver.h"

int main(int argc, char *argv[])
{
    int sock;                     /* Socket */
    struct sockaddr_in servAddr; 	/* Local address */
    struct sockaddr_in clntAddr; 	/* Client address */
    unsigned int cliAddrLen;      /* Length of incoming message */
    char *buffer;        					/* Buffer for string */
    char *read_buffer;
    u16 servPort;     	/* Server port */
    int recvMsgSize;              /* Size of received message */
		int listen = NULL;
		int rcvCount, sendCount = 0;

    if (argc != 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

	/* Port to listen to */
    servPort = atoi(argv[1]);

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
  
		/*handle intial request*/
    if ((recvMsgSize = recvfrom(sock, buffer, sizeof(request), 0,
        (struct sockaddr *) &clntAddr, &cliAddrLen)) == sizeof(request))
        {
          printf("Buffer: %s\n", buffer);
        }
    if (sendto(sock, buffer, recvMsgSize, 0, 
         (struct sockaddr *) &clntAddr, sizeof(clntAddr)) != recvMsgSize)
    {
        printf("Answering client.\n");
    }
		/*transfer*/

    for (;;) /* Run forever */
    {
      /* Set the size of the in-out parameter */
      cliAddrLen = sizeof(clntAddr);

      /* Block until receive message from a client */
      while((recvMsgSize = recvfrom(sock, buffer, sizeof(request), 0,
          (struct sockaddr *) &clntAddr, &cliAddrLen)) == sizeof(request))
			{
				rcvCount++;
				printf("Receive Count Incremented: %d\n", rcvCount);
			  /*else
			  {
          DieWithError("recvfrom() failed");
			  }*/
        printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));
        request *r = extract_request(buffer);
        read_buffer = malloc(sizeof(r->max_packet_size - PACKET_HEADER_SIZE));
        read(r->filename, read_buffer, (r->max_packet_size - PACKET_HEADER_SIZE));

        /* Send received datagram back to the client */
        if (sendto(sock, buffer, recvMsgSize, 0, 
             (struct sockaddr *) &clntAddr, sizeof(clntAddr)) != recvMsgSize)
			  {
        	DieWithError("sendto() sent a different number of bytes than expected");
			  }
			  else{
				  sendCount++;
				  printf("Receive Count Incremented: %d\n", sendCount);
			  }
		  }
    }
    /* NOT REACHED */
}

void transfer()
{

}
