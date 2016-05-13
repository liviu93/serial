#include "utils.h"

const char *cmdstr[] = {
	"help", 
	"exec", 
	"get", 
	"check"
};

#define FERROR "No such file or directory"
#define RERROR "Execution failed"
#define BUFSIZE 4096
char buff[BUFSIZE];

int main(void)
{
	info();
	
	struct cmd * cm = malloc(sizeof *cm);
	struct res *rs = res_init("/dev/pts/10");

	for (;;) {
		printf(">>");
		char *line = 0;
		size_t len = 0;
		ssize_t read = getline(&line, &len, stdin);
		if (read > 1) {
			line[read-1] = 0;
			cmd_tokenize(cm, line);

			if (!cm->cm_valid) {
				info();
				continue;
			}
	
			run_cmd(rs->fd, cm, line);
		}
	}

}	



void remote_file(int fd, const struct cmd *cm, const char * line)
{

	send_buf(fd, line + cm->cm_prm1.offset, 
			cm->cm_prm1.size, CGET);

	struct packet pkt;
	int num = 1;
	for(;;) {
		int pkt_ret = get_pkt(fd, &pkt, 0);
		
		if (pkt.header == NAK) {
			memcpy(buff, FERROR, strlen(FERROR));
			break;
		}

		if (pkt.header == ACK) break;

		if (pkt_ret != 0 || pkt.num != num || crc_ok(&pkt) != 0) {
			printf("[!] get_pkt\n");
			send_cmd(fd, num, NAK);
		} else if (pkt.header == CGET) {
			size_t offset = (num-1) * DATSIZ;
			memcpy(buff + offset, pkt.data, DATSIZ);
			buff[offset + DATSIZ] = 0; 
			num++;
			//print_pkt(&pkt);
		}
	}


}


void remote_exec(int fd, const struct cmd *cm, const char * line)
{
	/*
	 * Send the command string
	 * */
	
	send_buf(fd, line + cm->cm_prm1.offset, 
			cm->cm_prm1.size, CEXEC);
	
	if (cm->cm_prm2.size != 0)
		send_buf(fd, line + cm->cm_prm2.offset, 
				cm->cm_prm2.size, CPARAM);
	else
		send_buf(fd, " ", 1, CPARAM);

	/*
	 * Receive data
	 * */	

	struct packet pkt;
	int num = 1;
	for(;;) {
		int pkt_ret = get_pkt(fd, &pkt, 0);

		if (pkt.header == NAK) {
			memcpy(buff, RERROR, strlen(RERROR));
			break;
		}

		if (pkt.header == ACK) break;

		if (pkt_ret != 0 || pkt.num != num || crc_ok(&pkt) != 0) {
			printf("[!] get_pkt\n");
			send_cmd(fd, num, NAK);
		} else if (pkt.header == CEXEC) {
			size_t offset = (num-1) * DATSIZ;
			memcpy(buff + offset, pkt.data, DATSIZ);
			buff[offset + DATSIZ] = 0; 
			num++;
			//print_pkt(&pkt);
		}
	}

	printf("\n");

}

int check_conn(int fd) 
{
	send_cmd(fd, 0, CHK);
	struct packet pkt;

	if (get_pkt(fd, &pkt, 500) != 0) {
		printf("[!] get_pkt\n");
	}

	if (pkt.header == RSP)
		return 0;

	return -1;
}

void run_cmd(int fd, const struct cmd *cm, const char *line)
{

	switch (cm->cm_type) {
		case CEXEC:
			if (cm->cm_prm1.size > 0 ) {
				printf("-> [%.*s] [%.*s]\n", 
						(int)cm->cm_prm1.size, line + cm->cm_prm1.offset, 
						(int)cm->cm_prm2.size, line + cm->cm_prm2.offset
				);
				buff[0] = 0;
				remote_exec(fd, cm, line);
				printf("--> %s\n", buff);
			}
			else info();
		break;

		case CGET:
			if (cm->cm_prm1.size > 0) {
				printf("-> [%.*s]\n", (int)cm->cm_prm1.size, 
						line + cm->cm_prm1.offset);
				buff[0] = 0;
				remote_file(fd, cm, line);
				printf("--> %s\n", buff);
			}
			else info();
		break;

		case CCHECK:
			if (check_conn(fd) == 0)
				printf("--> OK\n");
			else printf("--> !!\n");
				
		break;

		case CHELP:
			info();
		break;

		default:
		break;

	}
}

void cmd_tokenize(struct cmd *cm, const char *line)
{
	const char *tmp = line;
	const char *ptmp;

	while (isspace(*tmp))
		tmp++;
	
	cm->cm_valid = false;
	for (size_t i = 0; i < MAX_CMD; i++) {
		size_t len = strlen(cmdstr[i]);

		if (strncmp(tmp, cmdstr[i], len) == 0) {
			cm->cm_type = i;	
			tmp += len;

			/* get command */
			while(*tmp && isspace(*tmp))
				tmp++;
			cm->cm_prm1.offset = tmp - line;
			ptmp = tmp;
			while (*tmp && !isspace(*tmp))
				tmp++;
			cm->cm_prm1.size = tmp - ptmp;

			/* get parameter */
			while(*tmp && isspace(*tmp))
				tmp++;
			cm->cm_prm2.offset = tmp - line;
			ptmp = tmp;
			while(*tmp) 
				tmp++;
			cm->cm_prm2.size = tmp - ptmp;

			cm->cm_valid = true;

			break;
		}

	}
	
}


void info(void) 
{
	printf(
			"*********************\n"
			"*-Available command-*\n"
			"*********************\n"
			"\tcheck [check connection]\n"
			"\tget <filename> [try to receive the specified file]\n"
			"\texec <shellcommand> [exec a command on a remote shell]\n"
			"\thelp [prints this text]\n"
			);
}
