CC = gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -g
LIBS = freetype2 fontconfig gl
CFLAGS += $(shell pkg-config --cflags $(LIBS))
LDFLAGS = $(shell pkg-config --libs $(LIBS))

LIB_NAME = lato
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

.PHONY: all clean test test_so

all: lib$(LIB_NAME).a lib$(LIB_NAME).so

lib$(LIB_NAME).a: $(OBJ_FILES)
	@ar rcs $@ $^
	@ranlib $@

lib$(LIB_NAME).so: $(OBJ_FILES)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $(OBJ_FILES) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

test: lib$(LIB_NAME).a
	@$(CC) $(CFLAGS) tests/main.c -L. -l:lib$(LIB_NAME).a -o test $(LDFLAGS)
	@./test

test_so: lib$(LIB_NAME).so
	@$(CC) $(CFLAGS) tests/main.c -L. -l:lib$(LIB_NAME).so -o test $(LDFLAGS)
	@LD_LIBRARY_PATH=$(shell pwd) ./test

clean:
	@rm -f lib$(LIB_NAME).a lib$(LIB_NAME).so
	@rm -f $(OBJ_DIR)/*.o
	@rm -f test
