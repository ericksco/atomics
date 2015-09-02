all:	foo

foo.o:	
	gcc -m64 -c foo.c

libatomics.so:
	gcc -m64 -Wall -Werror -fPIC -shared -o libatomics.so atomics.c

foo:	foo.o libatomics.so
	gcc -o foo -L. -latomics foo.o

clean:
	rm -rf *.o *.so
