.PHONY: all clean

TARGET = example_program
FILE_INFO_TARGET = file_info

CC = gcc

CFLAGS = -Wall -Wextra -g

SRCS = example_program.c file_info.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET) $(FILE_INFO_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(FILE_INFO_TARGET): file_info.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) $(FILE_INFO_TARGET) $(OBJS)
