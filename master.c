#include "utils.h"
#include <string.h>

//int creat_wr_pkt(void *data, short num, struct packet *pkt)

int main(void)
{

	struct res *rs = res_init();
	// res_set(rs);
	//
	//
	

	char data[] = "Whenever the receiver "
		"receives a packet, the receiver calculates exactly "
		"the same checksum or CRC, then comparesi"
		"it to the one in the footer/trailer. If they match,"
		"the entire packet is (almost certainly) good, so the receiver sends an ACK.";

	struct packet pkt;
	/*
	pkt.header = 'O';
	pkt.crc[0] = 'A';
	pkt.crc[1] = 'B';
	pkt.num = 0;
	memcpy(pkt.data, data, DATSIZ);
	
	
	for(int i = 0; i<1000; i++) {
	
		write(rs->fd[WR], &pkt, sizeof(struct packet));

		//char buf[3];
		//sleep(2);
		//int rd = read(rs->fd[RD], buf, 2);
		//tcflush(rs->fd[RD], TCIFLUSH);

		//if (strncmp(buf, "OK", 2) == 0){
		//	printf("OK\n");
		//} else {
		//	printf("BAD PKT %*.s\n", rd, buf);
		//}

		//getchar();

	}
	*/
	
	int num = 0;
	int ok = 0;
	do {
		pkt.tail = TAIL;
		pkt.header = ACK;
		pkt.num = ++num;
		int offset = num - 1;
		int to_send = strlen(data) - (offset * DATSIZ);
		memset(pkt.data, 0, DATSIZ);

		if (to_send > DATSIZ) {
			ok = 1;
			memcpy(pkt.data, data + offset * DATSIZ, DATSIZ);
		} else {
			ok = 0;
			memcpy(pkt.data, data + offset * DATSIZ, to_send);
		}

		pkt.crc = get_crc(pkt.data);

		write(rs->fd[WR], &pkt, sizeof(struct packet));
		tcdrain(rs->fd[WR]);
		sleep(1);

		int bytes = 0;
		ioctl(rs->fd[RD], FIONREAD, &bytes);
/*
		if (bytes != 0) 
		{
			short pkt_num;
			read(rs->fd[RD], &pkt_num, sizeof(short));
			printf("Read bytes = %d\n", pkt_num);
			num = pkt_num;
		} else
		{
			printf("Ok\n");
		}
*/
	print_pkt(&pkt);
	//getchar();
		
	} while (ok != 0);





	return 0;
}


