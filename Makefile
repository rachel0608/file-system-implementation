CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lreadline -lpthread

all: mysh 

mysh: mysh.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f mysh