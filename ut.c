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
	/* the following parameters aren't used:
 	* msg_size
 	* mbx_size
 	*/		

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
			ret = -1;
		}
	}
	/* now open fifo */
	if ( (*fd = open(fifoname, O_RDWR | O_NONBLOCK)) < 0 ) {
		perror("can't open fifo");
		ret = -1;
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
			ret = -1;
		}
	}

	/* this function should close the fd and delete the named pipe */
	close(fd);

	return 0;
}

/* open named pipe based on path name */
int UTASNMBX(char *mbx_name, int *fd)
{
	short	ret = 0;	/* return code */
	
	/* check whether mbx_name is populated */
	if ( strlen(mbx_name) > 0 ) {
		/* check whether fifo exists */
		if ( access(mbx_name, F_OK) == 0 ) {
			/* open fifo */
			if ( (*fd = open(mbx_name, O_RDWR | O_NONBLOCK)) < 0 ) {
				perror("can't open fifo");
				ret = -1;
			}
		}
		else
		{
			perror("fifo is inaccessible");
			ret = -1;
		}
	}
	else
	{
		perror("fifo is not named");
		ret = -1;
	}

	return ret;
}

/* close named pipe */
int UTDEAMBX(int fd)
{
	short	ret = 0;	/* return code */

	/* close file descriptor */
	close(fd);

	return ret;
}

/* read data from named pipe */
int UTREDMBX(int fd, size_t msg_size, char *wait_sec, struct vms_iosb *iosb, char *data)
{
	char	*dataptr = data;	/* pointer to passed in data buffer for use in write() function */
	ssize_t	read_return = 1;	/* holds quantity of read data */
	size_t	totalread = 0;		/* total quantity written */
	short	ret = 0;		/* return code */
	int	wait_sec_int;		/* integer translation of wait_sec */
	fd_set	readset;		/* holds fds for select() */
	struct 	timeval tv;		/* timeout structure */
	int	select_ready;		/* return code for select() */

	/* initialize counter */
	iosb->io_status = 0;
	iosb->chars_transferred = 0;
	/* grab read timeout */
	wait_sec_int = atoi(wait_sec);

	/* set timout */
	tv.tv_sec = wait_sec_int;
	tv.tv_usec = 0;

	/* add fd to read set */
	FD_ZERO(&readset);
	FD_SET(fd, &readset);

	while ( (select_ready = select(fd + 1, &readset, 0, 0, &tv)) > 0	/* fd is readable */
		&& totalread < msg_size 	/* msg_size data has not been read */
		&& read_return > 0) {		/* read() doesn't error or reach EOF */

		/* read from named pipe */
		if ( (read_return = read(fd, dataptr, msg_size - totalread)) != -1 )
		{
			/* successful read, set status */
			iosb->io_status = SS$_SUCCESS;
			iosb->chars_transferred += read_return;
			
			/* increment read pointer and counter */
			dataptr += read_return;
			totalread += read_return;

			/* add fd to read set for next call to select() */
			FD_ZERO(&readset);
			FD_SET(fd, &readset);
		}
		else /* read failure */
		{
			/* unsuccessful read, set status, get ready to fail */
			iosb->io_status = SS$_ABORT;
			perror("read failure");
			ret = -1;
		}
	}

	if (select_ready == 0) {	/* select() (read) timeout */
		/* set status, get ready to fail */
		fprintf(stderr, "read timeout\n");
		iosb->io_status = SS$_ABORT;
		ret = -2;
	}
	else if (select_ready == -1) {	/* select() error */
		/* set status, get ready to fail */
		perror("select failure");
		iosb->io_status = SS$_ABORT;
		ret = -1;
	}

	return ret;
}

/* write to named pipe */
int UTWRIMBX(int fd, size_t msg_size, char eof_ind, char *wait_sec, char *data, struct vms_iosb *iosb)
{
	char	*dataptr = data;	/* pointer to passed in data buffer for use in write() function */
	char	null = 0;
	ssize_t	write_return = 0;	/* holds return value from write() */
	size_t	totalwritten = 0;	/* total quantity written */
	short	ret = 0;		/* return code */
	int	wait_sec_int;		/* integer translation of wait_sec */
	fd_set	writeset;		/* holds fds for select() */
	struct 	timeval tv;		/* timeout structure */
	int	select_ready;		/* return code for select() */

	/* initialize counter */
	iosb->io_status = 0;
	iosb->chars_transferred = 0;

	/* grab read timeout */
	wait_sec_int = atoi(wait_sec);

	/* set timout */
	tv.tv_sec = wait_sec_int;
	tv.tv_usec = 0;

	/* add fd to read set */
	FD_ZERO(&writeset);
	FD_SET(fd, &writeset);

	/* if eof_ind = "Y", send only a single null and set iosb.io_status */
	if (eof_ind == 'Y') {
		dataptr = &null;
		msg_size = 1;
		iosb->io_status = SS$_ENDOFFILE;	
	}

	while ( (select_ready = select(fd + 1, 0, &writeset, 0, &tv)) > 0	/* fd is writable */
		&& (msg_size - totalwritten) > 0 				/* there's data left to write */
		&& write_return != -1) {					/* write() is not returning errors */

		if ( (write_return = write(fd, dataptr, msg_size - totalwritten)) > -1 ) {
			/* successful write, set status */
			iosb->io_status = (iosb->io_status == SS$_ENDOFFILE ? SS$_ENDOFFILE : SS$_SUCCESS);
			iosb->chars_transferred = totalwritten;

			/* increment read pointer and counter */
			dataptr += write_return;
			totalwritten += write_return;

			/* add fd to read set for next call to select() */
			FD_ZERO(&writeset);
			FD_SET(fd, &writeset);

			fprintf(stderr, "write returned: %d\n", (int) write_return);
		}
		else /* write failure */
		{
			/* unsuccessful read, set status, get ready to fail */
			iosb->io_status = SS$_ABORT;
			fprintf(stderr, "write failure: %d\n", (int) write_return);
			// perror("write failure: %d", (int) write_return);
			ret = -1;
		}
	}

	if (select_ready == 0) {	/* select() timeout */
		/* set status, get ready to fail */
		fprintf(stderr, "write timeout\n");
		iosb->io_status = SS$_ABORT;
		ret = -2;
	}
	else if (select_ready == -1) {	/* select() error */
		/* set status, get ready to fail */
		perror("select failure");
		iosb->io_status = SS$_ABORT;
		ret = -1;
	}

	return ret;
}
