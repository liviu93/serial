#include "utils.h"

static int open_dev(const char *path, int flags) 
{
	int fd = open(path, flags | O_NOCTTY);

	return fd;
}

struct res * res_init()
{
	struct res *rs = malloc(sizeof *rs);

	rs->fd[WR] = open_dev(WR_PATH, O_WRONLY);
	rs->fd[RD] = open_dev(RD_PATH, O_RDONLY);
	assert(rs->fd[WR] > 0 || rs->fd[RD] > 0);
	
    /* 
     * BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
     * CRTSCTS : output hardware flow control (only used if the cable has
     * all necessary lines. See sect. 7 of Serial-HOWTO)
     * CS8     : 8n1 (8bit,no parity,1 stopbit)
     * CLOCAL  : local connection, no modem contol
     * CREAD   : enable receiving characters
     */
	struct termios newtio;

    newtio.c_cflag = B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_cflag &= ~PARENB;
    newtio.c_cflag &= ~CSTOPB;
    newtio.c_cflag &= ~CSIZE;
    /*
    *           IGNPAR  : ignore bytes with parity errors
	*           ICRNL   : map CR to NL (otherwise a CR input on the other computer
    *                     will not terminate input)
    *                     otherwise make device raw (no other input processing)
    **/
    newtio.c_iflag = IGNPAR | ICRNL;
                               
    newtio.c_oflag = 0;
                                        
	newtio.c_lflag = ICANON;

    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0;     /* del */
	newtio.c_cc[VKILL]    = 0;     /* @ */
	newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
	newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
	newtio.c_cc[VSWTC]    = 0;     /* '\0' */
	newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
	newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	newtio.c_cc[VEOL]     = 0;     /* '\0' */
	newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	newtio.c_cc[VEOL2]    = 0;     /* '\0' */

	tcflush(rs->fd[WR], TCIFLUSH);
	tcsetattr(rs->fd[WR],TCSANOW,&newtio);

	tcflush(rs->fd[RD], TCIFLUSH);
	tcsetattr(rs->fd[RD],TCSANOW,&newtio);

	return rs;
}



void print_pkt(struct packet *pkt)
{
		printf("==================");
		printf("NUM = %d\n", pkt->num);
		printf("HEADER = %u\n", pkt->header);
		printf("CRC = %d\n", pkt->crc);
		printf("DATA = %.*s\n", DATSIZ, pkt->data);
		printf("==================\n");
}


short get_crc(const char *data) 
{
	int count = DATSIZ;
	short crc = 0;
	char i;

	while (--count >= 0) {
		char  tmp = data[count] + 1;
	    crc = crc ^ tmp << 8;
	    i = 8;
	    do {
			if (crc & 0x8000)
	           	crc = crc << 1 ^ 0x1021;
	    	else
				crc = crc << 1;
        } while(--i);
	}

	return crc;
}
