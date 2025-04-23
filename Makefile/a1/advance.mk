.PHONY: hello hellomake

CC := gcc
CFLAGS := -I .

INC_FILES := hello.h

#i%.o: %.c $(INC_FILES)
#	$(CC) -I. -c -o $@ $<

# tương tự
# main.o: main.c
# 	gcc -I. -c -o main.o main.c



test: hello.h hello.c
	@echo $@
	@echo $<
	@echo $^

hellomake: main.o hello.o
	$(CC) -o $@ main.o hello.o $(CFLAGS)

hello:
	gcc -o hello  main.c hello.c -I .

clean:
	rm -rf hello hellomake *.o


