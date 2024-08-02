CC = gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -g

LIBS = freetype2 fontconfig gl
CFLAGS += $(shell pkg-config --cflags $(LIBS))
LDFLAGS = $(shell pkg-config --libs $(LIBS))

LIB_NAME = lato
SRC_FILES = src/Lato.c src/math.c src/Character.c
OBJ_FILES = $(SRC_FILES:.c=.o)
INCLUDE_DIR = include

.PHONY: all clean test

all: lib$(LIB_NAME).a

lib$(LIB_NAME).a: $(OBJ_FILES)
	@ar rcs $@ $^
	@ranlib $@

%.o: src/%.c
	@$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

test: all
	@$(CC) tests/main.c -L. -l$(LIB_NAME) -o ./tests/test $(LDFLAGS)
	tests/test

clean:
	@rm -f lib$(LIB_NAME).a $(OBJ_FILES) ./tests/test
