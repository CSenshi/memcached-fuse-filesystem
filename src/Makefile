CC=gcc
COMPILE_FLAGS=-c
EXEC_FILE_FS=cachefs
LIB=`pkg-config fuse3 --cflags --libs`

all: cachefs.o memcached.o filesys.o dir.o utils.o chunk.o 
	$(CC) cachefs.o memcached.o filesys.o dir.o utils.o chunk.o debug.h -o $(EXEC_FILE_FS) $(LIB)

cachefs.o: cachefs.c
	$(CC) $(COMPILE_FLAGS) cachefs.c $(LIB)

memcached.o: memcached.c
	$(CC) $(COMPILE_FLAGS) memcached.c

filesys.o: filesys.c
	$(CC) $(COMPILE_FLAGS) filesys.c $(LIB)

file.o: file.c
	$(CC) $(COMPILE_FLAGS) file.c

dir.o: dir.c
	$(CC) $(COMPILE_FLAGS) dir.c

utils.o: utils.c
	$(CC) $(COMPILE_FLAGS) utils.c

chunk.o: chunk.c
	$(CC) $(COMPILE_FLAGS) chunk.c

clean:
	rm *.o $(EXEC_FILE_FS)