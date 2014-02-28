#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

char *port = "/dev/ttyACM0";
char  *progname;

#define MAXLINE 256

int
main(int argc, char *argv[])
{
  progname = argv[0];
	port = argv[1];

  int    fd;
  fd = open(port, O_RDWR | O_NOCTTY);
	if (fd == -1)
		perror(progname);

	struct termios options;
	tcgetattr(fd, &options); /* Get current terminal settings */

	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);

	options.c_cflag |= (CLOCAL | CREAD); 
	options.c_cflag &= ~(PARENB | CSTOPB);
	options.c_cflag &= ~CSIZE; 
	options.c_cflag |= CS8; 
	options.c_cflag &= ~CRTSCTS;
	options.c_iflag &= ~(IXON | IXOFF | IXANY); 

	options.c_lflag |= ~ISIG; 
	options.c_lflag |= ICANON; /* Enable canonical input */

	tcsetattr(fd, TCSANOW, &options);

	usleep(1500*1000); /* Give Arduino time to reset */
	tcflush(fd, TCIFLUSH);

	char buf[MAXLINE];
	while (read(fd, buf, 128) >= 0) {
		printf("%s", buf);
		memset(buf, 0, MAXLINE);
	}

	close(fd);
  return 0;
}
