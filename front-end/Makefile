CC = gcc
CFLAGS = -w
LDFLAGS = -lreadline

all: shell my_ls

shell: shell.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

my_ls: my_ls.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f shell my_ls
