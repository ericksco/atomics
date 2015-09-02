#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern int error();

/* create mailbox */
/* UTCREMBX.COB - This module creates a VMS mailbox */
int UTCREMBX(char *mbx_name, int msg_size, int mbx_size, int fd)
{
	/* check whether mbx_name is populated */
	if ( strlen(mbx_name) > 0 ) {
		/* check whether fifo exists */
		if ( access(mbx_name, F_OK) == -1 ) {
			/* create fifo */
			if ( mkfifo(mbx_name, 0644) < 0 )
				error("can't create fifo");
		}
		/* open fifo */
		if ( (fd = open(mbx_name, O_RDWR)) < 0 )
			error("can't open fifo");
		
	}

	/* need code for when no mbx_name is passed */
	return fd;
}



