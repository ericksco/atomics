foo.o:
	gcc -m64 -c foo.c

atomics.o:
	gcc -m64 -c -Wall -Werror -fpic atomics.c

atomics.so:
	gcc -shared -o libatomics.so atomics.o

foo:
	gcc -o foo -latomics foo.o

all:	foo

clean:	rm -rf *.o *.so
