#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>

#define SS$_SUCCESS     0
#define SS$_ABORT       1
#define SS$_CANCEL      2

struct vms_iosb
{
        int io_status;
        int chars_transferred;
        int dev_spec_info;
};
