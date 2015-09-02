all:	foo

foo.o:	foo.c foo.h
	gcc -m64 -Wall -Werror -c foo.c

libatomics.so:	foo.h atomics.c
	gcc -m64 -Wall -Werror -fPIC -shared -o libatomics.so atomics.c

foo:	foo.o libatomics.so
	gcc -o foo -L. -latomics foo.o

clean:
	rm -rf *.o *.so

test:	foo
	LD_LIBRARY_PATH=. ./foo
