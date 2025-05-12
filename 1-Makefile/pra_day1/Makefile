.PHONY: obj_static obj_shared create_static create_shared linking_static linking_shared main_static main_shared clean

CC := gcc



CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
INC_DIR := $(CUR_DIR)/inc
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
LIB_DIR := $(CUR_DIR)/lib
LIB_SHARED_DIR := $(LIB_DIR)/shared
LIB_STATIC_DIR := $(LIB_DIR)/static


obj_static:
	$(CC) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o -I $(INC_DIR)
	$(CC) -c $(SRC_DIR)/ex1.c -o $(OBJ_DIR)/ex1_static.o -I $(INC_DIR)

obj_shared:
	$(CC) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o -I $(INC_DIR)
	$(CC) -c -fPIC $(SRC_DIR)/ex1.c -o $(OBJ_DIR)/ex1_shared.o -I $(INC_DIR)

create_static: 
	ar rcs $(LIB_STATIC_DIR)/libex1_static.a $(OBJ_DIR)/ex1_static.o

create_shared: 
	$(CC)	-shared	$(OBJ_DIR)/ex1_shared.o -o $(LIB_SHARED_DIR)/libex1_shared.so

linking_static: 
	$(CC)   $(OBJ_DIR)/main.o  -L$(LIB_STATIC_DIR) -lex1_static -o $(BIN_DIR)/ex1_static_linked

linking_shared: 
	$(CC)	$(OBJ_DIR)/main.o -L$(LIB_SHARED_DIR) -lex1_shared -o $(BIN_DIR)/ex1_shared_linked

main_static: obj_static create_static linking_static
	@echo "Ex1 Static Library - Done!\n"

main_shared: obj_shared create_shared linking_shared
	@echo "Ex1 shared Library - Done!\n"
clean:
	rm -rf $(LIB_STATIC_DIR)/* $(LIB_SHARED_DIR)/* $(OBJ_DIR)/* $(BIN_DIR)/*  





