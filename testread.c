#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <time.h>
#include <sys/select.h>

int main()
{
	int fd;
	int ret;
	char buf[128];

	if ( (fd = open("/tmp/testfifo", O_RDWR | O_NONBLOCK )) < 0 ) {
		perror("can't open /tmp/testfifo");
		exit(1);
	}

	printf("opened\n");

	if ((ret = read(fd, buf, sizeof(buf))) < 0) {
		perror("failure to read");
		exit(1);
	}

	printf("read returned %d\n", ret);

	return 0;
}
