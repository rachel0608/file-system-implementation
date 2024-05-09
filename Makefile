CC = gcc
CFLAGS = -Wall -Wextra

all: format shell filesystem

format: format.c
	$(CC) $(CFLAGS) -o format format.c

shell: shell.c
	$(CC) $(CFLAGS) -o shell shell.c -lreadline

filesystem: filesystem.c
	$(CC) $(CFLAGS) -o filesystem filesystem.c

clean:
	rm -f format shell filesystem
