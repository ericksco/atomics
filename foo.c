#include "foo.h"

int error(char *msg)
{
        perror(msg);
        exit(1);
}

int main(int argc, char *argv[])
{
	int myfd;
	int retc;

	retc = UTCREMBX("/tmp/bob", 64, 64, &myfd);
	printf("fd = %d, retc = %d\n", myfd, retc);

	retc = UTDELMBX(myfd);
	printf("fd = %d, retc = %d\n", myfd, retc);
	
	return 0;
}
