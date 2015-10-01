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
	short retc;

	retc = UTCREMBX("/tmp/foo_reader", 64, 64, &myfd1);
        if ( retc < 0 ) {
                fprintf(stderr, "UTCREMBX returned non-zero\n");
                exit(1);
        }
	retc = UTDELMBX(myfd1);
        if ( retc < 0 ) {
                fprintf(stderr, "UTDELMBX returned non-zero\n");
                exit(1);
        }

	return 0;
}
