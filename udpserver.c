#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define MAX 255     /* Longest string */

void DieWithError(char *errorMessage);  /* External error handling function */

int main(int argc, char *argv[])
{
    int sock;                       /* Socket */
    struct sockaddr_in servAddr; 	/* Local address */
    struct sockaddr_in clntAddr; 	/* Client address */
    unsigned int cliAddrLen;        /* Length of incoming message */
    char buffer[MAX];        		/* Buffer for string */
    unsigned short servPort;     	/* Server port */
    int recvMsgSize;                /* Size of received message */

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
  
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(clntAddr);

        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, buffer, MAX, 0,
            (struct sockaddr *) &clntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");

        printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));

        /* Send received datagram back to the client */
        if (sendto(sock, buffer, recvMsgSize, 0, 
             (struct sockaddr *) &clntAddr, sizeof(clntAddr)) != recvMsgSize)
            DieWithError("sendto() sent a different number of bytes than expected");
    }
    /* NOT REACHED */
}
