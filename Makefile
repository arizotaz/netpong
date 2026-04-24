# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

NETPONG_SRC = main.c court.c $(wildcard network/*.c)

LIBS = -lncurses


# Object files
NETPONG_OBJ = $(NETPONG_SRC:.c=.o)

# Default target
all: netpong

# Build NetPong
netpong: $(NETPONG_OBJ)
	$(CC) $(CFLAGS) -o netpong $(NETPONG_OBJ) $(LIBS)

# Generic compile rule
%.o: %.c smsh.h varlib.h
	$(CC) $(CFLAGS) -c $<

# Clean up
clean:
	rm -f *.o netpong network/*.o

Server:
	./netpong

Client:
	./netpong -c 127.0.0.1