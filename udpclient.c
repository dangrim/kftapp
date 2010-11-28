#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() and alarm() */
#include <errno.h>      /* for errno and EINTR */
#include <signal.h>     /* for sigaction() */

#define MAX 	        255     /* Longest string */
#define TIMEOUT_SECS    2       /* Seconds between retransmits */
#define MAXTRIES        5       /* Tries before giving up */

int tries=0;   /* Count of times sent - GLOBAL for signal-handler access */

void DieWithError(char *errorMessage);   /* Error handling function */
void CatchAlarm(int ignored);            /* Handler for SIGALRM */

int main(int argc, char *argv[])
{
    int sock;                       /* Socket descriptor */
    struct sockaddr_in servAddr; 	/* Server address */
    struct sockaddr_in fromAddr;    /* Source address */
    unsigned short servPort;     	/* Server port */
    unsigned int fromSize;          /* In-out of address size for recvfrom() */
    struct sigaction myAction;      /* For setting signal handler */
    char *servIP;                   /* IP address of server */
    char *string;                	/* String to send to server */
    char buffer[MAX+1];      		/* Buffer for string */
    int stringLen;               	/* Length of string */
    int respStringLen;         		/* Size of received datagram */

if (argc != 4)    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP>  <Port #> <Expression>\n", argv[0]);
        exit(1);
    }

	servIP = argv[1];           /* First arg:  server IP address (dotted quad) */
	servPort = atoi(argv[2]);   /* Second arg: port # */
    string = argv[3];       /* Third arg: expression */

    if ((stringLen = strlen(string)) > MAX)
        DieWithError("Word too long");

    /* Create a best-effort datagram socket using UDP */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Set signal handler for alarm signal */
    myAction.sa_handler = CatchAlarm;
    if (sigfillset(&myAction.sa_mask) < 0) /* block everything in handler */
        DieWithError("sigfillset() failed");
    myAction.sa_flags = 0;

    if (sigaction(SIGALRM, &myAction, 0) < 0)
        DieWithError("sigaction() failed for SIGALRM");

    /* Construct the server address structure */
    memset(&servAddr, 0, sizeof(servAddr));    /* Zero out structure */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    servAddr.sin_port = htons(servPort);       /* Server port */

    /* Send the string to the server */
    if (sendto(sock, string, stringLen, 0, (struct sockaddr *)
               &servAddr, sizeof(servAddr)) != stringLen)
        DieWithError("sendto() sent a different number of bytes than expected");
  
    /* Get a response */
    
    fromSize = sizeof(fromAddr);
    alarm(TIMEOUT_SECS);        /* Set the timeout */
    while ((respStringLen = recvfrom(sock, buffer, MAX, 0,
           (struct sockaddr *) &fromAddr, &fromSize)) < 0)
        if (errno == EINTR)     /* Alarm went off  */
        {
            if (tries < MAXTRIES)      /* incremented by signal handler */
            {
                printf("timed out, %d more tries...\n", MAXTRIES-tries);
                if (sendto(sock, string, stringLen, 0, (struct sockaddr *)
                            &servAddr, sizeof(servAddr)) != stringLen)
                    DieWithError("sendto() failed");
                alarm(TIMEOUT_SECS);
            } 
            else
                DieWithError("No Response");
        } 
        else
            DieWithError("recvfrom() failed");

    /* recvfrom() got something --  cancel the timeout */
    alarm(0);

    /* null-terminate the received data */
    buffer[respStringLen] = '\0';
    printf("Received: %s\n", buffer);    /* Print the received data */
    
    close(sock);
    exit(0);
}

void CatchAlarm(int ignored)     /* Handler for SIGALRM */
{
    tries += 1;
}
