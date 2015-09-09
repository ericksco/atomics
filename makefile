# 
# Description:
#
#      Library to emulate VMS mailboxes using named pipes (FIFOs)
#
# Log:
#
#      Date            Author                  Change Desc
#      ===========     =============           ====================
#      01-SEP-2015     Cory Erickson           Creation
#


all:	libut.so

libut.so:	ut.h ut.c
	gcc -g -m64 -Wall -Werror -fPIC -shared -o libut.so ut.c

foo_reader:	foo_reader.c libut.so
	gcc -g -m64 -o foo_reader -L. -lut foo_reader.c

foo_writer:	foo_writer.c libut.so
	gcc -g -m64 -o foo_writer -L. -lut foo_writer.c

clean:
	rm -rf *.o *.so foo_reader foo_writer

test:	foo_reader foo_writer
	LD_LIBRARY_PATH=. ./foo_reader & \
	sleep 2; \
	LD_LIBRARY_PATH=. ./foo_writer
