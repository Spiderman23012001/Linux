.PHONY: all clean

TARGET = example_program

CC = gcc

CFLAGS = -Wall -Wextra -g

SRCS = example_program.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) $(OBJS)
