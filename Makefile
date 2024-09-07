# Variables
CC = gcc
SRC = main.c
BIN_DIR = bin
BIN = $(BIN_DIR)/main.out
CFLAGS = -Wall -pthread

.PHONY: all run build clean

all: build

build:
	@mkdir -p $(BIN_DIR)
	$(CC) $(SRC) -o $(BIN) $(CFLAGS)

run: build
	./$(BIN)

clean:
	rm -rf $(BIN_DIR)

