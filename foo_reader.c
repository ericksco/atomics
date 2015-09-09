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
	printf("foo_reader: listening on fd %d (retc = %d)\n", myfd1, retc);

sleep(10);

	retc = UTREDMBX(myfd1, sizeof(buffer) - 1, "10", &iosb, buffer);
	buffer[sizeof(buffer)] = '\0';
	printf("foo_reader: read %lu bytes from fd %d: \"%s\"\n", sizeof(buffer), myfd1, buffer);
	printf("foo_reader: UTREDMBX returned %d\n", retc);
	printf("foo_reader: iosb.iostatus = %d\n", iosb.io_status);
	printf("foo_reader: iosb.chars_transferred = %d\n", iosb.chars_transferred);

	retc = UTDELMBX(myfd1);
	printf("foo_reader: UTDELMBX returned %d\n", retc);

	return 0;
}
