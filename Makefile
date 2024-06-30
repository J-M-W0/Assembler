CC := clang
CFLAGS := -c
CFLAGS += -Wall 
CFLAGS += -I./inc
LFLAGS := -Wall

LIB_DIR := lib
SRC_DIR := src

OBJ := $(LIB_DIR)/main.o 
OBJ += $(LIB_DIR)/rasm.o 
OBJ += $(LIB_DIR)/str.o 
OBJ += $(LIB_DIR)/utils.o 
OBJ += $(LIB_DIR)/table.o 
OBJ += $(LIB_DIR)/instru.o 
OBJ += $(LIB_DIR)/operand.o 
OBJ += $(LIB_DIR)/assemble.o 
OBJ += $(LIB_DIR)/codegen.o 
OBJ += $(LIB_DIR)/resources.o 
TARGET = rasm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LFLAGS) -o $@ $^

$(LIB_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJ)

