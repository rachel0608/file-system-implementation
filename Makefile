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

# CC = gcc
# CFLAGS = -Wall -Wextra

# # source files for each target
# SOURCES_FORMAT = format.c fat.c
# SOURCES_SHELL = shell.c
# SOURCES_FAKE_DISK_1 = fake_disk_1.c
# SOURCES_FAKE_DISK_2 = fake_disk_2.c
# SOURCES_FILESYSTEM = filesystem.c fat.c

# # output executables
# EXEC_FORMAT = format
# EXEC_SHELL = shell
# EXEC_FAKE_DISK_1 = fake_disk_1
# EXEC_FAKE_DISK_2 = fake_disk_2
# EXEC_FILESYSTEM = filesystem

# all: $(EXEC_FORMAT) $(EXEC_SHELL) $(EXEC_FAKE_DISK_1) $(EXEC_FAKE_DISK_2) $(EXEC_FILESYSTEM)

# $(EXEC_FORMAT): $(SOURCES_FORMAT)
# 	$(CC) $(CFLAGS) -o $@ $^

# $(EXEC_SHELL): $(SOURCES_SHELL)
# 	$(CC) $(CFLAGS) -o $@ $^ -lreadline

# $(EXEC_FAKE_DISK_1): $(SOURCES_FAKE_DISK_1)
# 	$(CC) $(CFLAGS) -o $@ $^

# $(EXEC_FAKE_DISK_2): $(SOURCES_FAKE_DISK_2)
# 	$(CC) $(CFLAGS) -o $@ $^

# $(EXEC_FILESYSTEM): $(SOURCES_FILESYSTEM)
# 	$(CC) $(CFLAGS) -o $@ $^

# run_disks:
# 	./$(EXEC_FAKE_DISK_1)
# 	./$(EXEC_FAKE_DISK_2)

# clean:
# 	rm -f $(EXEC_FORMAT) $(EXEC_SHELL) $(EXEC_FAKE_DISK_1) $(EXEC_FAKE_DISK_2) $(EXEC_FILESYSTEM)
