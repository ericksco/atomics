/*
 *
 * Description:
 * 
 * 	Library to emulate VMS mailboxes using named pipes (FIFOs)
 *
 * Log:
 * 	
 * 	Date		Author			Change Desc
 *	===========	=============		====================
 * 	01-SEP-2015	Cory Erickson		Creation
 *
 */

#include "ut.h"

/* creates named pipe */
int UTCREMBX(char *mbx_name, int msg_size, int mbx_size, int *fd)
{
	short	ret = 0;		/* return code */
	long	mytime;			/* time holder for fifo name */
	pid_t	mypid;			/* holds pid for fifo name */
	char	newfifo[PATH_MAX];	/* holds new mbx_name */
	char	*fifoname;		/* pointer to fifo name */

	/* ignoring msg_size and mbx_size */

	/* check whether mbx_name is populated */
	if ( strlen(mbx_name) > 0 ) {
		/* set pointer */
		fifoname = mbx_name;
	}
	else	/* no mbx_name is passed, create a unique fifo (fd will be used by program to look up fifo name via readlink) */
	{
		/* fabricate a fifo name based on pid and timestamp */
		/* we must cast these for printf use below */
		mytime = (long) time(0);
		mypid = (int) getpid();

		snprintf(newfifo, sizeof(newfifo), "/tmp/utfifo-%d-%ld", mypid, mytime);

		/* set pointer */
		fifoname = newfifo;
	}

	/* check whether fifo exists */
	if ( access(fifoname, F_OK) == -1 ) {
		/* doesn't exist; create fifo */
		if ( mkfifo(fifoname, 0644) < 0 ) {
			perror("can't create fifo");
			ret = 1;
		}
	}
	/* now open fifo */
	if ( (*fd = open(fifoname, O_RDWR)) < 0 ) {
		perror("can't open fifo");
		ret = 1;
	}
		
	return ret;
}

/* delete named pipe */
int UTDELMBX(int fd)
{
	char fdlink[PATH_MAX];		/* file descriptor file link in /proc/self */
	char pipepath[PATH_MAX];	/* fifo pathname */
	int pathlength;			/* size of fifo pathname */
	short	ret = 0;		/* return code */
	
	sprintf(fdlink, "/proc/self/fd/%d", fd);
	if ( (pathlength = readlink(fdlink, pipepath, sizeof(pipepath) - 1)) > 0 ) {
		pipepath[pathlength] = '\0';
		if ( unlink(pipepath) < 0 ) {
			perror("can't unlink pipe");
			ret = 1;
		}
	}

	/* this function should close the fd and delete the named pipe */
	close(fd);

	return 0;
}

int UTASNMBX(char *mbx_name, int *fd)
{
	short	ret = 0;	/* return code */
	
	/* check whether mbx_name is populated */
	if ( strlen(mbx_name) > 0 ) {
		/* check whether fifo exists */
		if ( access(mbx_name, F_OK) == 0 ) {
			/* open fifo */
			if ( (*fd = open(mbx_name, O_RDWR)) < 0 ) {
				perror("can't open fifo");
				ret = 1;
			}
		}
		else
		{
			perror("fifo is inaccessible");
			ret = 1;
		}
	}
	else
	{
		perror("fifo is not named");
		ret = 1;
	}

	return ret;
}

int UTDEAMBX(int fd)
{
	short	ret = 0;	/* return code */

	/* close file descriptor */
	close(fd);

	return ret;
}

int UTREDMBX(int fd, size_t msg_size, int wait_sec, struct vms_iosb *iosb, char *data)
{
	size_t	read_return = 0;	/* holds quantity of read data */
	short	ret = 0;		/* return code */

	/* how much to read? */
	read_return = read(fd, data, msg_size);

	/* update iosb */
	if ( read_return < 0 ) {
		perror("read failure");
		iosb->io_status = SS$_ABORT;
		iosb->chars_transferred = 0;
		ret = 1;
	}
	else
	{
		iosb->io_status = SS$_SUCCESS;
		iosb->chars_transferred = read_return;
		ret = 0;
	}

	return ret;
}

int UTWRIMBX(int fd, size_t msg_size, int eof_ind, int wait_sec, char *data, struct vms_iosb *iosb)
{
	char	*dataptr = data;	/* pointer to passed in data buffer for use in write() function */
	size_t	write_return = 0;	/* holds return value from write() */
	size_t	totalwritten = 0;	/* total quantity written */
	short	ret = 0;		/* return code */

	while ( ((msg_size - totalwritten) > 0) && (write_return != -1) ) {
		if ( (write_return = write(fd, dataptr, msg_size - totalwritten)) == -1 ) {
			perror("write failure");

			/* update iosb */
			iosb->io_status = SS$_ABORT;
			iosb->chars_transferred = totalwritten;
			ret = 1;
		}
		dataptr += write_return;
		totalwritten += write_return;
	}

	/* update iosb */
	iosb->io_status = SS$_SUCCESS;
	iosb->chars_transferred = totalwritten;
	
	return ret;
}
