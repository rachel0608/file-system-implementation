CC=gcc
CFLAGS=-Wall -g

EXECUTABLES=my_ls 

all: $(EXECUTABLES)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLES)

.PHONY: all clean
