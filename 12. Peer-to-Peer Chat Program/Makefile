CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRCS = src/main.c src/connection.c src/network.c src/message.c
OBJS = $(SRCS:.c=.o)
TARGET = chat

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
