
SRC_DIR = src
OBJ_DIR = objs
BIN_DIR = bin

CC = gcc
CFLAGS = -I include 
LDFLAGS = -L . -lwaveco 

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/kvstore

all: check_objs check_bin $(TARGET)

check_objs:
	if [ ! -d "objs" ]; then \
		mkdir -p objs;  \
	fi

check_bin:
	if [ ! -d "bin" ]; then \
		mkdir -p bin;   \
	fi

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $^ -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJS) $(TARGET)
