#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SS$_SUCCESS     0
#define SS$_ABORT       1
#define SS$_CANCEL      2

struct vms_iosb
{
        int io_status;
        int chars_transferred;
        int dev_spec_info;
};

int error(char *msg);
int UTCREMBX(char *mbx_name, int msg_size, int mbx_size, int *fd);
int UTDELMBX(int fd);
int UTASNMBX(char *mbx_name, int *fd);
int UTDEAMBX(int fd);
int UTWRIMBX(int fd, size_t msg_size, int eof_ind, int wait_sec, char *data, struct vms_iosb *iosb);
