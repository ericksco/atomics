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


all:	foo_reader foo_writer libut.so

foo_reader.o:	foo.h foo_reader.c
	gcc -g -m64 -Wall -Werror -c foo_reader.c

foo_writer.o:	foo.h foo_writer.c
	gcc -g -m64 -Wall -Werror -c foo_writer.c

libut.so:	ut.h ut.c
	gcc -g -m64 -Wall -Werror -fPIC -shared -o libut.so ut.c

foo_reader:	foo_reader.o libut.so
	gcc -g -m64 -o foo_reader -L. -lut foo_reader.o

foo_writer:	foo_writer.o libut.so
	gcc -g -m64 -o foo_writer -L. -lut foo_writer.o

clean:
	rm -rf *.o *.so foo_reader foo_writer

test:	foo_reader foo_writer
	LD_LIBRARY_PATH=. ./foo_reader & \
	sleep 5; LD_LIBRARY_PATH=. ./foo_writer
