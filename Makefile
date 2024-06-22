CC := clang
CFLAGS := -Wall 
CFLAGS += -c
LFLAGS := -Wall

OBJ := assembler.o 
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

