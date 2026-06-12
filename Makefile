.PHONY: all debug
CC     = gcc
CFLAGS = -Wall -Wextra

DEBUG_FLAGS = -g -O0 -DDEBUG
BUILD_FLAGS = -O2

# Default target builds optimized binaries
all: CFLAGS += $(BUILD_FLAGS)
all: build/game 

# Debug target builds with symbols
debug: CFLAGS += $(DEBUG_FLAGS)
debug: build/game 

build/game: src/game.c src/snl.c
	$(CC) $(CFLAGS) src/game.c src/snl.c -o build/game

