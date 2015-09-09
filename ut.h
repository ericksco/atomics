/*
*
* Description:
*
*      Library to emulate VMS mailboxes using named pipes (FIFOs)
*
* Log:
*
*      Date            Author                  Change Desc
*      ===========     =============           ====================
*      01-SEP-2015     Cory Erickson           Creation
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <time.h>
#include <sys/select.h>

#define SS$_SUCCESS     0
#define SS$_ABORT       1
#define SS$_CANCEL      2

struct vms_iosb
{
        int io_status;
        int chars_transferred;
        int dev_spec_info;
};

int UTCREMBX(char *mbx_name, int msg_size, int mbx_size, int *fd);
int UTDELMBX(int fd);
int UTASNMBX(char *mbx_name, int *fd);
int UTDEAMBX(int fd);
int UTWRIMBX(int fd, size_t msg_size, int eof_ind, char *wait_sec, char *data, struct vms_iosb *iosb);
int UTREDMBX(int fd, size_t msg_size, char *wait_sec, struct vms_iosb *iosb, char *data);
