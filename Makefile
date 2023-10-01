CC = gcc
CFLAGS = -Wall -Wextra -g
SRC_DIR = .
OUTPUT_DIR = output
BIN_DIR = ./$(OUTPUT_DIR)/bin
TARGET = ./$(OUTPUT_DIR)/my_program

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRCS))

.PHONY: all clean

all: $(BIN_DIR) $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJS)
	mkdir -p $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTPUT_DIR)

