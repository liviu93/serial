#include <unistd.h>
#include <stdio.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main(void)
{
	char dev[] = "/dev/ttyS0";

	int fd = open(dev, O_RDWR | O_NOCTTY);
	struct termios options;
   	tcgetattr(fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
	options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    //options.c_cflag &= ~( ICANON | ECHO | ECHOE |ISIG );
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
    		                | INLCR | IGNCR | ICRNL | IXON);
    options.c_oflag &= ~OPOST;

	//cfmakeraw(&options);

	options.c_cc[VTIME] = 0;
	options.c_cc[VMIN] = 5;
    
    tcsetattr(fd, TCSANOW, &options);



	int total = 0;
	char w[] = "ABCDE";
	for (;;) {
		int wr = write(fd, w, 5);
		total += wr;
		tcdrain(fd);

		int bytes = 0;
		ioctl(fd, FIONREAD, &bytes);
		printf("iqueue = %d\n", bytes); 

		printf("Written %d bytes [%d]\n", wr, total);
		sleep(1);
	}
	return 0;
}


