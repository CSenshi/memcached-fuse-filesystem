CC=gcc
COMPILE_FLAGS=-c -Wall
EXEC_FILE_FS=cachefs
LIB=`pkg-config fuse3 --cflags --libs`

cachefs: cachefs.o
	$(CC) cachefs.o -o $(EXEC_FILE_FS) $(LIB)

cachefs.o: cachefs.c
	$(CC) $(COMPILE_FLAGS) cachefs.c $(LIB)

clean:
	rm *.o $(EXEC_FILE_FS)