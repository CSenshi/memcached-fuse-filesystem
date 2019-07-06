CC=gcc
COMPILE_FLAGS=-c
EXEC_FILE_FS=cachefs
LIB=`pkg-config fuse3 --cflags --libs`

all: cachefs.o memcached.o filesys.o
	$(CC) cachefs.o memcached.o filesys.o debug.h -o $(EXEC_FILE_FS) $(LIB)

cachefs.o: cachefs.c
	$(CC) $(COMPILE_FLAGS) cachefs.c $(LIB)

memcached.o: memcached.c
	$(CC) $(COMPILE_FLAGS) memcached.c

filesys.o: filesys.c
	$(CC) $(COMPILE_FLAGS) filesys.c $(LIB)

clean:
	rm *.o $(EXEC_FILE_FS)