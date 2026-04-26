# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
LIBS = -lncurses

# Source files
NETPONG_SRC = main.c $(wildcard src/*.c) $(wildcard src/network/*.c)

# Object files in build/
NETPONG_OBJ = $(patsubst %.c,build/%.o,$(NETPONG_SRC))

# Default target
all: netpong

# Link
netpong: $(NETPONG_OBJ)
	$(CC) $(CFLAGS) -o $@ $(NETPONG_OBJ) $(LIBS)

# Compile rule (FULL PATH match)
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf build netpong

# Run targets
Server:
	./netpong 2001

Client:
	./netpong 127.0.0.1 2001