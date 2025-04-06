CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
LIB_DIR := $(CUR_DIR)/lib
INC_DIR := $(CUR_DIR)/inc
SRC_DIR := $(CUR_DIR)/src
OBJ_DIR := $(CUR_DIR)/obj
LIB_SHARED_DIR := $(LIB_DIR)/shared
LIB_STATIC_DIR := $(LIB_DIR)/static


# Note: fpic = position independent code
create_obj:
	gcc -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o -I $(INC_DIR)
	gcc -c -fPIC $(SRC_DIR)/myMath.c -o $(OBJ_DIR)/myMath.o -I $(INC_DIR)

create_shared:
	gcc -shared $(OBJ_DIR)/myMath.o -o $(LIB_SHARED_DIR)/libmyMath.so 

linking:
	gcc $(OBJ_DIR)/main.o -L$(LIB_SHARED_DIR) -lmyMath -o $(BIN_DIR)/shared_lib


all: create_obj create_shared linking
	@echo "create shared lib"



clean:
	rm -rf $(BIN_DIR)/shared_lib $(OBJ_DIR)/*.o $(BIN_DIR)/shared_lib $(LIB_SHARED_DIR)/*




