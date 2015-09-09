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
	struct vms_iosb iosb;	/* structure to hold IO metadata */
	char buffer[50];	/* read buffer */

	retc = UTCREMBX("/tmp/foo_reader", 64, 64, &myfd1);
        if ( retc < 0 ) {
                fprintf(stderr, "UTCREMBX returned non-zero");
                exit(1);
        }

	retc = UTREDMBX(myfd1, sizeof(buffer) - 1, "3", &iosb, buffer);
        if ( retc == -2 ) {
                printf("UTREDMBX timeout: SUCCESS!!");
        }

	retc = UTDELMBX(myfd1);
        if ( retc < 0 ) {
                fprintf(stderr, "UTDELMBX returned non-zero");
        }

	printf("\n");

	return 0;
}
