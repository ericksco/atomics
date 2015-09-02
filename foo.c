#include "foo.h"

int error(char *msg)
{
        perror(msg);
        exit(1);
}

int main(int argc, char *argv[])
{
	int myfd1;
	int myfd2;
	int retc;

	retc = UTCREMBX("/tmp/bob", 64, 64, &myfd1);
	printf("fd = %d, retc = %d\n", myfd1, retc);

	retc = UTASNMBX("/tmp/bob", &myfd2);
	printf("fd = %d, retc = %d\n", myfd2, retc);

	retc = UTDEAMBX(myfd2);
	printf("fd = %d, retc = %d\n", myfd2, retc);

	retc = UTDELMBX(myfd1);
	printf("fd = %d, retc = %d\n", myfd1, retc);
	
	return 0;
}
