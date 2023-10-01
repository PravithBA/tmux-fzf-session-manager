CC=gcc
CFLAGS=-I.

main: main.c
	$(CC) -o ./binary/main main.c
