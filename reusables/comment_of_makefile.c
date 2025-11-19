// CC      := gcc
// CFLAGS  := -Wall -Wextra -Werror -std=c11 -O2
// LDFLAGS :=
// SRC     := $(wildcard *.c)
// OBJ     := $(SRC:.c=.o)
// TARGET  := myprogram
//
// .PHONY: all clean debug
//
// all: $(TARGET)
//
// $(TARGET): $(OBJ)
// 	$(CC) $(OBJ) -o $@ $(LDFLAGS)
//
// debug: CFLAGS += -g -O0 -DDEBUG
// debug: $(TARGET)
//
// clean:
// 	rm -f $(OBJ) $(TARGET)
//
// %.o: %.c
// 	$(CC) $(CFLAGS) -c $< -o $@
