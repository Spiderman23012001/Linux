TOOLCHAIN := /usr/bin/

ifeq ($(ARCH), arm)
	CC := $(TOOLCHAIN)/gcc
endif

all:
	$(CC) -o hello hello.c 

clean:
	rm -rf hello