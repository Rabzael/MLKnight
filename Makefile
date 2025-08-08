project_name = mlknight
CC = gcc
CFLAGS = -Wall -O2
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
DBG_DIR = debug
TARGET = $(BIN_DIR)/$(project_name)

SRCS = $(wildcard $(SRC_DIR)/*.c $(INC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(filter $(SRC_DIR)/%.c,$(SRCS))) \
       $(patsubst $(INC_DIR)/%.c,$(OBJ_DIR)/%.o,$(filter $(INC_DIR)/%.c,$(SRCS)))

all: $(TARGET)

debug: CFLAGS += -g
debug: $(DBG_DIR)/$(project_name)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(DBG_DIR)/$(project_name): $(OBJS)
	@mkdir -p $(DBG_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DBG_DIR)

.PHONY: all clean debug