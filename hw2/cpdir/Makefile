CC = gcc
CFLAGS = -pthread -Wall

all: cpdir
	./cpdir src_dir dest_dir

cpdir: cpdir.c
	$(CC) $(CFLAGS) cpdir.c -o cpdir
