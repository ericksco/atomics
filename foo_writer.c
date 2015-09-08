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
	char buffer[50] = "five nights at freddies";	/* write buffer with arbitrary data */

	retc = UTASNMBX("/tmp/foo_reader", &myfd1);
	printf("foo_writer: attached to /tmp/foo_reader, fd = %d (retc = %d)\n", myfd1, retc);

	retc = UTWRIMBX(myfd1, sizeof(buffer), 0, 0, buffer, &iosb);
        printf("foo_writer: wrote %lu bytes to fd %d: \"%s\"\n", sizeof(buffer), myfd1, buffer);
        printf("foo_writer: UTWRIMBX returned %d\n", retc);
        printf("foo_writer: iosb.iostatus = %d\n", iosb.io_status);
        printf("foo_writer: iosb.chars_transferred = %d\n", iosb.chars_transferred);

	retc = UTDEAMBX(myfd1);
	printf("foo_writer: detached from /tmp/foo_reader (retc = %d)\n", retc);

	return 0;
}
