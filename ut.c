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
	if ( (*fd = open(fifoname, O_RDWR)) < 0 ) {
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

int UTDEAMBX(int fd)
{
	short	ret = 0;	/* return code */

	/* close file descriptor */
	close(fd);

	return ret;
}

int UTREDMBX(int fd, size_t msg_size, char *wait_sec, struct vms_iosb *iosb, char *data)
{
	/* the following parameters aren't used:
 	*/		

	size_t	read_return = -99;	/* holds quantity of read data */
	short	ret = 0;		/* return code */
	char	wait_sec_str[3];
	int	wait_sec_int;
	fd_set	readset;
	struct 	timeval tv;
	int	select_ready;

	/* grab read timeout */
	strncpy(wait_sec_str, wait_sec, sizeof(wait_sec_str));
	wait_sec_int = atoi(wait_sec_str);

	/* set timout */
	tv.tv_sec = wait_sec_int;
	tv.tv_usec = 0;

	/* add fd to read set */
	FD_ZERO(&readset);
	FD_SET(fd, &readset);

	select_ready = select(fd + 1, &readset, 0, 0, &tv);	

	if (select_ready > 0) {
		/* how much to read? */
		read_return = read(fd, data, msg_size);
		if ( read_return < 0 ) {
			perror("read failure");
			ret = -1;
		}
		else
		{
			iosb->io_status = SS$_SUCCESS;
			iosb->chars_transferred = read_return;
		}
	}
	else if (select_ready == 0) {
		fprintf(stderr, "read timeout");
		iosb->io_status = SS$_ABORT;
		iosb->chars_transferred = 0;
		ret = -1;
	}
	else /* ( select_ready < 0 ) */ {
		perror("select failure");
		ret = -1;
	}

	return ret;
}

int UTWRIMBX(int fd, size_t msg_size, int eof_ind, char *wait_sec, char *data, struct vms_iosb *iosb)
{
	/* the following parameters aren't used:
	* eof_ind is not utilized at this time 	
 	*/		

	char	*dataptr = data;	/* pointer to passed in data buffer for use in write() function */
	size_t	write_return = 0;	/* holds return value from write() */
	size_t	totalwritten = 0;	/* total quantity written */
	short	ret = 0;		/* return code */
	char	wait_sec_str[3];
	int	wait_sec_int;
	fd_set	writeset;
	struct 	timeval tv;
	int	select_ready;

	iosb->chars_transferred = 0;

	/* grab read timeout */
	strncpy(wait_sec_str, wait_sec, sizeof(wait_sec_str));
	wait_sec_int = atoi(wait_sec_str);

	/* set timout */
	tv.tv_sec = wait_sec_int;
	tv.tv_usec = 0;

	/* add fd to read set */
	FD_ZERO(&writeset);
	FD_SET(fd, &writeset);

	printf("in write\n");

	while ( (select_ready = select(fd + 1, 0, &writeset, 0, &tv)) > 0 && ((msg_size - totalwritten) > 0) && (write_return != -1) ) {
		if ( (write_return = write(fd, dataptr, msg_size - totalwritten)) == -1 ) {
			perror("write failure");

			/* update iosb */
			iosb->io_status = SS$_ABORT;
			iosb->chars_transferred = totalwritten;
			ret = -1;
		}
		printf("wrote %d bytes\n", (int) write_return);
		fflush(stdin);
		dataptr += write_return;
		totalwritten += write_return;

		/* add fd to read set */
		FD_ZERO(&writeset);
		FD_SET(fd, &writeset);
	}

	if (select_ready == 0) {
		fprintf(stderr, "write timeout");
		iosb->io_status = SS$_ABORT;
		ret = -1;
	}

	if ( write_return > 0 ) {
		/* update iosb */
		iosb->io_status = SS$_SUCCESS;
		iosb->chars_transferred = totalwritten;
	}
	
	return ret;
}
