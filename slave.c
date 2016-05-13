#include "utils.h"

#define BUFSIZE 4096

	char data[] = "Whenever the receiver "
		"receives a packet, the receiver calculates exactly "
		"the same checksum or CRC, then comparesi";
struct {
	int fd;
	off_t offset;
	char exec[BUFSIZE];
	char params[BUFSIZE];
	char out[BUFSIZE];
} cmds;

int exec_cmd()
{
	printf("[%s] [%s]\n", cmds.exec, cmds.params);
	
	FILE *fp;
	strcat(cmds.exec, " ");
	strcat(cmds.exec, cmds.params);
	strcat(cmds.exec, " 2>&1");
	printf("[%s] \n", cmds.exec);
	fp = popen(cmds.exec, "r");

	memset(cmds.out, 0, BUFSIZE);
	fread(cmds.out, BUFSIZE, 1, fp);
	pclose(fp);
	
	return 0;
}

int get_file()
{
	printf("[%s]\n", cmds.exec);
	int fd = open(cmds.exec, O_RDONLY);
	if (fd < 0)
		return -1;

	memset(&cmds.out , 0, BUFSIZE);
	int rdnum = read(fd, cmds.out, BUFSIZE);	
	//printf("%s\n", cmds.out);

	close(fd);
	return rdnum;
}

int main(void)
{

	struct res *rs = res_init("/dev/pts/11");
	
	
	printf("\tServer\n");


	struct packet pkt, prev_pkt;
	memset(&pkt, 0, sizeof(struct packet));
	memset(&prev_pkt, 0, sizeof(struct packet));

	int num = 1;

	for(;;) {
		prev_pkt = pkt;
		int pkt_ret = get_pkt(rs->fd, &pkt, 0);

		if (pkt.header == CHK) {
			send_cmd(rs->fd, 0, RSP);
			continue;
		}

		if (pkt.header == ACK) {
			printf("received ACK\n");
			if (prev_pkt.header == CPARAM) {
				if (exec_cmd() == -1)
					send_cmd(rs->fd, 0, NAK);
				else send_buf(rs->fd, cmds.out, sizeof(cmds.out), CEXEC);
			}
			if (prev_pkt.header == CGET) {
				if (get_file() == -1) 
						send_cmd(rs->fd, 0, NAK);
				else send_buf(rs->fd, cmds.out, sizeof(cmds.out), CGET);
			}

			num = 1;
			continue;
		}
		
		
		if (pkt_ret != 0 || pkt.num != num || crc_ok(&pkt) != 0) {
			printf("[!] get_pkt\n");
			send_cmd(rs->fd, num, NAK);
		} else if (pkt.header == CEXEC || pkt.header == CGET){
			printf("received CEXEC or CGET\n");
			size_t offset = (num-1) * DATSIZ;
			memcpy(cmds.exec + offset, pkt.data, DATSIZ);
			cmds.exec[offset + DATSIZ] = 0; 
			num++;
			//print_pkt(&pkt);
		} else if (pkt.header == CPARAM) {
			printf("received CPARAM\n");
			size_t offset = (num-1) * DATSIZ;
			memcpy(cmds.params + offset, pkt.data, DATSIZ);
			cmds.params[offset + DATSIZ] = 0; 
			num++;
			//print_pkt(&pkt);
		}

	}

	return 0;
}

