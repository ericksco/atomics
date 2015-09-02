#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>

extern int error();

/* creates named pipe */
int UTCREMBX(char *mbx_name, int msg_size, int mbx_size, int *fd)
{
	/* ignoring msg_size and mbx_size */

	/* check whether mbx_name is populated */
	if ( strlen(mbx_name) > 0 ) {
		/* check whether fifo exists */
		if ( access(mbx_name, F_OK) == -1 ) {
			/* create fifo */
			if ( mkfifo(mbx_name, 0644) < 0 ) {
				error("can't create fifo");
				return 1;
			}
		}
		/* open fifo */
		if ( (*fd = open(mbx_name, O_RDWR)) < 0 ) {
			error("can't open fifo");
			return 1;
		}
	}
	else
	{
		/* need code for when no mbx_name is passed */
		error("fifo unnamed");
		return 1;
	}

	return 0;
}

/* delete named pipe */
int UTDELMBX(int fd)
{
	char fdlink[PATH_MAX];
	char pipepath[PATH_MAX];
	int pathlength;
	
	sprintf(fdlink, "/proc/self/fd/%d", fd);
	if ( (pathlength = readlink(fdlink, pipepath, sizeof(pipepath) - 1)) > 0 ) {
		pipepath[pathlength] = '\0';
		if ( unlink(pipepath) < 0 ) {
			error("can't unlink pipe");
			return 1;
		}
	}

	/* this function should close the fd and delete the named pipe */
	close(fd);

	return 0;
}

int UTASNMBX(char *mbx_name, int *fd)
{
	
	/* check whether mbx_name is populated */
	if ( strlen(mbx_name) > 0 ) {
		/* check whether fifo exists */
		if ( access(mbx_name, F_OK) == 0 ) {
			/* open fifo */
			if ( (*fd = open(mbx_name, O_RDWR)) < 0 ) {
				error("can't open fifo");
				return 1;
			}
		}
		else
		{
			error("fifo is inaccessible");
			return 1;
		}
	}
	else
	{
		error("fifo is not named");
		return 1;
	}

	return 0;
}

int UTDEAMBX(int fd)
{
	close(fd);

	return 0;
}
