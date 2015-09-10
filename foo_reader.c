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
	char buffer[256];	/* read buffer */

	retc = UTCREMBX("/tmp/foo_reader", 64, 64, &myfd1);
        if ( retc < 0 ) {
                fprintf(stderr, "UTCREMBX returned non-zero\n");
                exit(1);
        }
	printf("foo_reader: listening on fd %d (retc = %d)\n", myfd1, retc);

	retc = UTREDMBX(myfd1, sizeof(buffer) - 1, "05", &iosb, buffer);
        if ( retc < 0 ) {
                fprintf(stderr, "UTREDMBX returned non-zero\n");
                exit(1);
        }
	buffer[sizeof(buffer)] = '\0';

	printf("foo_reader: read %lu bytes from fd %d: \"%s\"\n", sizeof(buffer), myfd1, buffer);
	printf("foo_reader: UTREDMBX returned %d\n", retc);
	printf("foo_reader: iosb.iostatus = %d\n", iosb.io_status);
	printf("foo_reader: iosb.chars_transferred = %d\n", iosb.chars_transferred);

	retc = UTDELMBX(myfd1);
        if ( retc < 0 ) {
                fprintf(stderr, "UTDELMBX returned non-zero\n");
                exit(1);
        }

	printf("foo_reader: UTDELMBX returned %d\n", retc);

	return 0;
}
