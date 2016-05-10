#include <unistd.h>
#include <stdio.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	char dev[] = "/dev/ttyS0";

	int fd = open(dev, O_RDWR | O_NOCTTY);
	fcntl(fd, F_SETFL, 0);
	struct termios tio;
	cfmakeraw(&tio);
	tio.c_cc[VTIME] = 0;
	tio.c_cc[VMIN] = 1;
	tio.c_cflag |= B9600;
	tcsetattr(fd, TCSAFLUSH, &tio);

	char buff[6];

	printf("Reading...\n");
	int total = 0;

	for (;;) {
		int rd = read(fd, buff, 5);
		total += rd;
		printf("Read %d bytes [%d]\n", rd, total);

		buff[5] = 0;	
		printf("%*.s\n", rd, buff);
	}

	return 0;
}


