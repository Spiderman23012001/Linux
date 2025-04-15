.PHONY: all run clean

CC = gcc
CFLAGS = -pthread

all: 
	$(CC) -o exam main.c $(CFLAGS)

run: exam
	./exam

clean: 
	rm -rf exam