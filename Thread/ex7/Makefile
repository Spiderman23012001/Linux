CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lsqlite3 -lpthread

all: sqlite_thread_demo

sqlite_thread_demo: sqlite_thread_demo.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f sqlite_thread_demo
