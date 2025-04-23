.PHONY: all server client clean

all: server.c
	gcc -o server server.c
	gcc -o client client.c

server: 
	./server

client:
	./client

clean:
	rm -rf server client