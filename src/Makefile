CC=gcc
COMPILE_FLAGS=-c
EXEC_FILE_FS=cachefs
LIB=`pkg-config fuse3 --cflags --libs`

cachefs: cachefs.o memcached.o
	$(CC) cachefs.o memcached.o debug.h -o $(EXEC_FILE_FS)

cachefs.o: cachefs.c
	$(CC) $(COMPILE_FLAGS) cachefs.c

memcached.o: memcached.c
	$(CC) $(COMPILE_FLAGS) memcached.c

clean:
	rm *.o $(EXEC_FILE_FS)