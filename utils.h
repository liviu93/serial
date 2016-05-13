#ifndef _UTILS_H
#define _UTILS_H



#include <unistd.h>
#include <stdio.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>



#define WR_PATH "/dev/pts/4"
#define RD_PATH "/dev/pts/8"

#define NDEV 2
#define WR 0
#define RD 1 


#define DATSIZ 8

struct res
{
	int fd;
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



struct res * res_init(const char * dev);
short get_crc(const char *data);
void print_pkt(struct packet *pkt);

int create_npkt(const void * buf, size_t size, 
			struct packet *pkt, size_t num, char header);
int crc_ok(struct packet *pkt);
int get_pkt(int fd, struct packet *pkt, int timeout);
int send_pkt(int fd, struct packet *pkt);

/*
 * Master
 *
 * */

#define MAX_CMD 4

#define CHELP 	0x0
#define CEXEC 	0x1
#define CGET	0x2
#define CCHECK 	0x3
#define CPARAM 	0x4


typedef struct {
	size_t offset;
	size_t size;
} token_t;

struct cmd {
	size_t cm_type; /* type */

	token_t cm_prm1;
	token_t cm_prm2;

	bool cm_valid; 
};

void info(void);
void cmd_tokenize(struct cmd *cm, const char *line);
void run_cmd(int fd, const struct cmd *cm, const char *line);

int get_bad_npkt(int fd);
void send_cmd(int fd, int num, char header);
void send_buf(int fd, const void * buf, 
				size_t size, char header);
#endif
