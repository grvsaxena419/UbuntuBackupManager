include ../Makefile.include

user_prog: main.o backup.o sqlite_db_functions.o restore.o helpers.o restorepoint.o
	$(CC) -Wall -g -o userprog main.o backup.o sqlite_db_functions.o restore.o helpers.o restorepoint.o -lsqlite3

main.o: main.c userprog.h
	$(CC) -c -g main.c
	
backup.o: backup.c userprog.h
	$(CC) -c -g backup.c

sqlite_db_functions.o: sqlite_db_functions.c userprog.h
	$(CC) -c -g sqlite_db_functions.c
	
restore.o: restore.c userprog.h
	$(CC) -c -g restore.c
	
helpers.o: helpers.c userprog.h
	$(CC) -c -g helpers.c
	
restorepoint.o: restorepoint.c userprog.h
	$(CC) -c -g restorepoint.c

run : userprog
	sudo ./userprog backup &
	
clean: 
	rm *.o
	rm userprog
