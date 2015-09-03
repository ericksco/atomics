#include "ut_lib.h"

/* creates named pipe */
int UTCREMBX(char *mbx_name, int msg_size, int mbx_size, int *fd)
{
	short	ret = 0;
	/* ignoring msg_size and mbx_size */

	/* check whether mbx_name is populated */
	if ( strlen(mbx_name) > 0 ) {
		/* check whether fifo exists */
		if ( access(mbx_name, F_OK) == -1 ) {
			/* create fifo */
			if ( mkfifo(mbx_name, 0644) < 0 ) {
				perror("can't create fifo");
				ret = 1;
			}
		}
		/* open fifo */
		if ( (*fd = open(mbx_name, O_RDWR)) < 0 ) {
			perror("can't open fifo");
			ret = 1;
		}
	}
	else
	{
		/* need code for when no mbx_name is passed */
		perror("fifo unnamed");
		ret = 1;
	}

	return ret;
}

/* delete named pipe */
int UTDELMBX(int fd)
{
	char fdlink[PATH_MAX];
	char pipepath[PATH_MAX];
	int pathlength;
	short	ret = 0;
	
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
	short	ret = 0;
	
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
	short	ret = 0;

	close(fd);

	return ret;
}

int UTREDMBX()
{
	short	ret = 0;

	return ret;
}

int UTWRIMBX(int fd, size_t msg_size, int eof_ind, int wait_sec, char *data, struct vms_iosb *iosb)
{
	size_t	*dataptr = data;
	size_t	write_return;
	size_t	totalwritten = 0;
	short	ret = 0;

	while ( (msg_size - totalwritten) > 0 ) {
		if ( (write_return = write(fd, dataptr, msg_size - totalwritten)) == -1 ) {
			perror("write failure");
			iosb->io_status = SS$_ABORT;
			iosb->chars_transferred = totalwritten;
			ret = 1;
		}
		dataptr += write_return;
		totalwritten += write_return;
	}

	iosb->io_status = SS$_SUCCESS;
	iosb->chars_transferred = totalwritten;
	
	return ret;
}

/*
before read or write, set iosb to '0'

char    char_iosb[18];
char    char_iosb_field[5];

strncpy(char_iosb_field,char_iosb,5);
in_iosb.condition = atoi(char_iosb_field);

if ((in_iosb.condition == SS$_CANCEL) || (in_iosb.condition == SS$_ABORT)) ...

static struct
{
    unsigned short int condition,count;
    long int dev;
}


01  ls-iosb.
    05  ls-iosb-status          pic 9(5).
    05  ls-chars-transferred    pic 9(5).
    05  ls-dev-spec-info        pic 9(8).


move QIO_STATUS         to ls-iosb-status
move CHARS_TRANSFERRED  to ls-chars-transferred
move DEV_SPEC_INFO      to ls-dev-spec-info

*/
