CC = gcc
CFLAGS = -Wall -Wextra

all: format shell filesystem fake_disk_1

format: format.c
	$(CC) $(CFLAGS) -o format format.c

shell: shell.c
	$(CC) $(CFLAGS) -o shell shell.c -lreadline

filesystem: filesystem.c
	$(CC) $(CFLAGS) -o filesystem filesystem.c

fake_disk: fake_disk_1.c
	$(CC) $(CFLAGS) -o fake_disk_1 fake_disk_1.c

clean:
	rm -f format shell filesystem fake_disk_1
