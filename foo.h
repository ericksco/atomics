#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int error(char *msg);

int UTCREMBX(char *mbx_name, int msg_size, int mbx_size, int *fd);
int UTDELMBX(int fd);
int UTASNMBX(char *mbx_name, int *fd);
int UTDEAMBX(int fd);
