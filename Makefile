CC = gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -g
LIBS = freetype2 fontconfig gl
CFLAGS += $(shell pkg-config --cflags $(LIBS))
LDFLAGS = $(shell pkg-config --libs $(LIBS))

LIB_NAME = lato
LIB_DIR = lib
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests
INCLUDE_DIR = include
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

.PHONY: all clean test test_so valgrind

all: $(LIB_DIR)/lib$(LIB_NAME).a $(LIB_DIR)/lib$(LIB_NAME).so

$(LIB_DIR)/lib$(LIB_NAME).a: $(OBJ_FILES)
	@mkdir -p lib
	@ar rcs $@ $^
	@ranlib $@

$(LIB_DIR)/lib$(LIB_NAME).so: $(OBJ_FILES)
	@mkdir -p lib
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $(OBJ_FILES) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

test: $(LIB_DIR)/lib$(LIB_NAME).a
	@$(CC) $(CFLAGS) tests/main.c -L. -l:$(LIB_DIR)/lib$(LIB_NAME).a -o test $(LDFLAGS) -lglfw
	@./test

test_so: $(LIB_DIR)/lib$(LIB_NAME).so
	@$(CC) $(CFLAGS) tests/main.c -L. -l:$(LIB_DIR)/lib$(LIB_NAME).so -o test $(LDFLAGS) -lglfw
	@LD_LIBRARY_PATH=$(shell pwd) ./test

valgrind: test
	@valgrind --leak-check=full ./test

clean:
	@rm -rf $(OBJ_DIR) $(LIB_DIR) test
