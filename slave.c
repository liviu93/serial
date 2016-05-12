#include "utils.h"
#include <string.h>

int main(void)
{

	struct res *rs = res_init();
	
	
	printf("START!!!!!!\n");
	printf("START!!!!!!\n");


	struct packet_ctl pkt_ctl = {0};



	for(;;) {
		
		pkt_ctl.prev_pkt = pkt_ctl.curr_pkt;
		size_t pkt_size = sizeof(struct packet);
		int rdnum = read(rs->fd[RD], &pkt_ctl.curr_pkt, pkt_size);
		printf("\tPREV\n");
		print_pkt(&pkt_ctl.prev_pkt);
		printf("\tCURR\n");
		print_pkt(&pkt_ctl.curr_pkt);

		short new_crc = get_crc(pkt_ctl.curr_pkt.data);


		if (pkt_ctl.curr_pkt.num - pkt_ctl.prev_pkt.num != 1) {
			printf("diff != 1 \n");
			continue;
		}

		if (rdnum != pkt_size) {
			printf("rdnum != pkt_size \n");
			continue;
		}

		if (new_crc != pkt_ctl.curr_pkt.crc) {
			printf("crc != new_crc \n");
			continue;
		}

		if (pkt_ctl.curr_pkt.header != ACK) {
			printf("header != ACK \n");
			continue;
		}
/*
		if (
				rdnum != pkt_size ||
				pkt_ctl.curr_pkt.num - pkt_ctl.prev_pkt.num != 1 || 
				new_crc != pkt_ctl.curr_pkt.crc ||
				pkt_ctl.curr_pkt.header != ACK
			) {

			//short next_num = pkt_ctl.prev_pkt.num + 1;
			//write(rs->fd[WR], &next_num, sizeof(short));
			//tcdrain(rs->fd[WR]);

			printf("WRONG PACKET !\n");
			continue;
		}
*/




		//int bytes = 0;
		//do
		//{
		//	ioctl(rs->fd[RD], FIONREAD, &bytes);
		//	printf("bytes after write %d\n", bytes);
		//	sleep(1);
		//} while (bytes == 0);

	}

	return 0;
}


