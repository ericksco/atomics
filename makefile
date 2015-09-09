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

foo_reader_timeout:	foo_reader_timeout.c libut.so
	gcc -g -m64 -o foo_reader_timeout -L. -lut foo_reader_timeout.c

foo_writer_timeout:	foo_writer_timeout.c libut.so
	gcc -g -m64 -o foo_writer_timeout -L. -lut foo_writer_timeout.c

clean:
	rm -rf *.o *.so foo_reader foo_writer

test:	foo_reader_timeout foo_writer_timeout foo_reader foo_writer
