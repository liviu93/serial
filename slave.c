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

	int fd = open(dev, O_RDWR);
	
	char buff[2];
	read(fd, buff, 1);
	buff[1] = 0;	
	
	printf("%s\n", buff);

	return 0;
}


