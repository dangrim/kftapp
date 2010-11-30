#ifndef _KFTSERVER_H__
#define _KFTSERVER_H__

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <string.h>
#include <signal.h>
#include <errno.h>
#include "dropper.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define ACK 1
#define NACK 0
#define TIMEOUT_VALUE 	100000
#define INITIAL_REQUEST_SIZE 3
#define RESPONSE_HEADER_SIZE 8
#define MAX_REQUEST_SIZE 60000


void accept_request();
void make_pkt();
void init_transfer(u8 *buffer, u32 msg_size);
int send_until_success();
void CatchAlarm(int ignored);
u32 unpack_int(u8 *buffer);
int read_a_file(char *filename, u8 *buffer, u16 read_length);

#endif
