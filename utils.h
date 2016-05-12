#ifndef _UTILS_H
#define _UTILS_H



#include <unistd.h>
#include <stdio.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>


#define WR_PATH "/dev/pts/4"
#define RD_PATH "/dev/pts/8"

#define NDEV 2
#define WR 0
#define RD 1 


#define DATSIZ 32

struct res
{
	int fd[NDEV];
};

#define TAIL '\n'

/*
 * HEADER
 */
#define ACK 0x12
#define NAK 0x23
#define CHK 0x34
#define RSP 0x32

struct packet 
{
	char header;
	short crc;
	short num;
	char data[DATSIZ];
	char tail;
} __attribute__((__packed__));

struct packet_ctl
{
	struct packet prev_pkt;
	struct packet curr_pkt;
};



struct res * res_init();

short get_crc(const char *data);

void print_pkt(struct packet *pkt);


#endif
