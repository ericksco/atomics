all:	foo

foo.o:	foo.h foo.c
	gcc -m64 -Wall -Werror -c foo.c

libut_lib.so:	ut_lib.h ut_lib.c
	gcc -m64 -Wall -Werror -fPIC -shared -o libut_lib.so ut_lib.c

foo:	foo.o libut_lib.so
	gcc -o foo -L. -lut_lib foo.o

clean:
	rm -rf *.o *.so

test:	foo
	LD_LIBRARY_PATH=. ./foo
