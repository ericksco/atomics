/*
*
* Description:
*
*      Test program for ut library
*
* Log:
*
*      Date            Author                  Change Desc
*      ===========     =============           ====================
*      01-SEP-2015     Cory Erickson           Creation
*
*/

#include "foo.h"

int main(int argc, char *argv[])
{
	int myfd1;	/* file descriptor */
	int retc;	/* return code */
	struct vms_iosb iosb;	/* structure to IO metadata */
	char buffer1[128] = "five nights at freddies";	/* write buffer with arbitrary data smaller than PIPE_BUF */
	char buffer2[128 * 1024] = "five nights at freddies";	/* write buffer with arbitrary data larger than PIPE_BUF */

	retc = UTASNMBX("/tmp/foo_reader", &myfd1);
	if ( retc < 0 ) {
		fprintf(stderr, "UTASNMBX returned non-zero\n");
		exit(1);
	}

	/* there should be no reader -- small buffer */
	retc = UTWRIMBX(myfd1, sizeof(buffer1), 0, "02", buffer1, &iosb);
	if ( retc == -2 ) {
		printf("UTWRIMBX returned non-zero: SUCCESS!!\n");
	}

	retc = UTWRIMBX(myfd1, sizeof(buffer2), 0, "02", buffer2, &iosb);
	if ( retc == -2 ) {
		printf("UTWRIMBX returned non-zero: SUCCESS!!\n");
	}

	retc = UTDEAMBX(myfd1);
	if ( retc < 0 ) {
		fprintf(stderr, "UTDEAMBX returned non-zero\n");
		exit(1);
	}

	return 0;
}
