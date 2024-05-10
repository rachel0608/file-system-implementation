CC = gcc
CFLAGS = -Wall -Wextra

all: format shell filesystem fake_disk

format: format.c
	$(CC) $(CFLAGS) -o format format.c

shell: shell.c
	$(CC) $(CFLAGS) -o shell shell.c -lreadline

filesystem: filesystem.c
	$(CC) $(CFLAGS) -o filesystem filesystem.c

fake_disk: fake_disk.c
	$(CC) $(CFLAGS) -o fake_disk fake_disk.c

clean:
	rm -f format shell filesystem fake_disk
