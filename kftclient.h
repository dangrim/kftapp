#ifndef _KFTCLIENT_H__
#define _KFTCLIENT_H__

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <signal.h>
#include <errno.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define ACK 1
#define NACK 0
#define INITIAL_REQUEST_SIZE 3
#define REQUEST_SIZE 5

char *pack_init_buffer(char *fname, int fname_length, u16 max_p_size);
int receive_and_send();
void write_msg();
void CatchAlarm(int ignored);
void make_request();
int unpack_int(u8 *buffer);

#endif
