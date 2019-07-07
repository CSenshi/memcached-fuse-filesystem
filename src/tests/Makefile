CC=gcc
COMPILE_FLAGS=-c
EXEC_FILE_MEMCACHED=memcached_test


memcached_test: memcached_test.o memcached.o utils.o
	$(CC) memcached_test.o memcached.o utils.o -o $(EXEC_FILE_MEMCACHED)

memcached.o: ../memcached.c
	$(CC) $(COMPILE_FLAGS) ../memcached.c

utils.o: ../utils.c
	$(CC) $(COMPILE_FLAGS) ../utils.c

memcached_test.o: memcached_test.c
	$(CC) $(COMPILE_FLAGS) memcached_test.c

clean:
	rm *.o $(EXEC_FILE_MEMCACHED)