#include "utils.h"

static int open_dev(const char *path, int flags) 
{
	int fd = open(path, flags | O_NOCTTY);

	return fd;
}

struct res * res_init(const char * dev)
{
	struct res *rs = malloc(sizeof *rs);

	rs->fd = open_dev(dev, O_RDWR);
	assert(rs->fd > 0);
	
    /* 
     * BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
     * CRTSCTS : output hardware flow control (only used if the cable has
     * all necessary lines. See sect. 7 of Serial-HOWTO)
     * CS8     : 8n1 (8bit,no parity,1 stopbit)
     * CLOCAL  : local connection, no modem contol
     * CREAD   : enable receiving characters
     */
    /*
	struct termios newtio;

    newtio.c_cflag = B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_cflag &= ~PARENB;
    newtio.c_cflag &= ~CSTOPB;
    newtio.c_cflag &= ~CSIZE;
    */
    /*
    *           IGNPAR  : ignore bytes with parity errors
	*           ICRNL   : map CR to NL (otherwise a CR input on the other computer
    *                     will not terminate input)
    *                     otherwise make device raw (no other input processing)
    **/

    //newtio.c_iflag = IGNPAR | ICRNL;
                               
    //newtio.c_oflag = 0;
                                        
	//newtio.c_lflag = ICANON;

    //newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
    //newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    //newtio.c_cc[VERASE]   = 0;     /* del */
	//newtio.c_cc[VKILL]    = 0;     /* @ */
	//newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
	//newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	//newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
	//newtio.c_cc[VSWTC]    = 0;     /* '\0' */
	//newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
	//newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	//newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	//newtio.c_cc[VEOL]     = 0;     /* '\0' */
	//newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	//newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	//newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	//newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	//newtio.c_cc[VEOL2]    = 0;     /* '\0' */

	//tcflush(rs->fd, TCIFLUSH);
	//tcsetattr(rs->fd,TCSANOW,&newtio);

	return rs;
}



void print_pkt(struct packet *pkt)
{
		printf("==================");
		printf("NUM = [%d]\n", pkt->num);
		printf("HEADER = [%u]\n", pkt->header);
		printf("CRC = [%d]\n", pkt->crc);
		printf("DATA = [%.*s]\n", DATSIZ, pkt->data);
		printf("==================\n");

	//char *buf = (char*)pkt;

	//for(int i = 0; i<sizeof(struct packet); i++) {
	//	printf("%02X ", buf[i] & 0xFF);
	//}
	//printf("\n================== %lu\n", sizeof(struct packet));
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

/*
 * -1 - last packet
 * 0 - not the last packet
 * */
int create_npkt(const void * buf, size_t size, 
			struct packet *pkt, size_t num, char header) 
{

	pkt->header = header;
	pkt->num = num;
	size_t offset = num - 1;
	size_t to_send = size - (offset * DATSIZ);
	memset(pkt->data, 0, DATSIZ);

	int ret = 0;
	if (buf != NULL) {
		if (to_send > DATSIZ) {
			memcpy(pkt->data, buf + offset * DATSIZ, DATSIZ);
			ret = 0;
		} else {
			memcpy(pkt->data, buf + offset * DATSIZ, to_send);
			ret = -1;
		}
	}
	pkt->crc = get_crc(pkt->data);

	return ret;
}

int send_pkt(int fd, struct packet *pkt)
{
	size_t size = sizeof(struct packet);
	int wrnum = write(fd, pkt, size);
	tcdrain(fd);

	if (wrnum != size)
		return -1;
	return 0;
}


int get_pkt(int fd, struct packet *pkt, int timeout)
{
  fd_set set;
  struct timeval tout;
  int rv, rdnum = 0, size = 0 ;

  FD_ZERO(&set); /* clear the set */
  FD_SET(fd, &set); /* add our file descriptor to the set */

  tout.tv_sec = 0;
  tout.tv_usec = timeout;

  if (timeout == 0)
  	rv = select(fd + 1, &set, NULL, NULL, NULL);
  else
	  rv = select(fd + 1, &set, NULL, NULL, &tout);

  if(rv > 0 ) {
  	  size = sizeof(struct packet);
  	  rdnum = read(fd, pkt, size);
  }
  if (rdnum != size)
  	  return -1;
  return 0;
}

int crc_ok(struct packet *pkt)
{
	short crc = get_crc(pkt->data);

	if (crc != pkt->crc)
		return -1;
	return 0;
}


void send_cmd(int fd, int num, char header)
{
	struct packet pkt;
	memset(&pkt, 0, sizeof(struct packet));
	create_npkt(NULL, 0, &pkt, 0, header);


	if (send_pkt(fd, &pkt) != 0) 
		printf("[!] send_pkt\n");
}


int get_bad_npkt(int fd)
{
	int bytes = 0;
	ioctl(fd, FIONREAD, &bytes);
	
	if (bytes != 0) {
		struct packet pkt;
		int ret = get_pkt(fd, &pkt, 0);

		//printf("\tBAD PACKET [%d]!\n", pkt.num);

		if (ret == 0 || pkt.header == NAK)
			return pkt.num;
	}

	return 0;
}


void send_buf(int fd, const void * buf, 
				size_t size, char header)
{
	int ret;
	struct packet pkt;
	int num = 1;

	do {
		ret = create_npkt(buf, size, &pkt, num, header);
		if (send_pkt(fd, &pkt) != 0) 
			printf("[!] send_pkt\n");

		int num_pkt = get_bad_npkt(fd);
		if (num_pkt != 0)
			num = num_pkt;
		else num++;


		//print_pkt(&pkt);
	} while (ret != -1);

	send_cmd(fd, 0, ACK);
}
