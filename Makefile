CC = gcc
CFLAGS = -Wall -Wextra

all: format shell fake_disk_1 fake_disk_2 filesystem

frontend: shell.c
	$(CC) -Wextra -o shell shell.c -lreadline

backend: format.c fake_disk_1.c fake_disk_2.c filesystem.c
	$(CC) $(CFLAGS) -o format format.c
	$(CC) $(CFLAGS) -o fake_disk_1 fake_disk_1.c
	$(CC) $(CFLAGS) -o fake_disk_2 fake_disk_2.c
	$(CC) $(CFLAGS) -o filesystem filesystem.c

format: format.c
	$(CC) $(CFLAGS) -o format format.c

shell: shell.c
	$(CC) $(CFLAGS) -o shell shell.c -lreadline

filesystem: filesystem.c
	$(CC) $(CFLAGS) -o filesystem filesystem.c

fake_disks: fake_disk_1.c fake_disk_2.c
	$(CC) $(CFLAGS) -o fake_disk_1 fake_disk_1.c
	$(CC) $(CFLAGS) -o fake_disk_2 fake_disk_2.c

fake_disk_1: fake_disk_1.c
	$(CC) $(CFLAGS) -o fake_disk_1 fake_disk_1.c

fake_disk_2: fake_disk_2.c
	$(CC) $(CFLAGS) -o fake_disk_2 fake_disk_2.c

run_disks:
	./fake_disk_1
	./fake_disk_2

clean:
	rm -f format shell filesystem fake_disk_1 fake_disk_2
