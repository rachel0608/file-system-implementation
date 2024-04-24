CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lreadline

all: shell

shell: shell.c
    $(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
   rm -f shell *.o
