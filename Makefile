CC := clang
CFLAGS := -c
CFLAGS += -Wall 
CFLAGS += -I./inc
LFLAGS := -Wall

OBJ := assembler.o 
OBJ += src/utils.o
OBJ += src/str.o
OBJ += src/table.o
OBJ += src/instru.o
OBJ += res/resource.o
TARGET = assembler

.PHONY: release clean

release: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(TARGET)
	rm -rf $(OBJ)

