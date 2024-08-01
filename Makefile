CC = gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -g

# Use pkg-config to get the necessary compile and link flags
CFLAGS += $(shell pkg-config --cflags freetype2 fontconfig)
LDFLAGS = $(shell pkg-config --libs freetype2 fontconfig)

LIB_NAME = liblato
OBJ_FILES = src/Lato.o src/math.o src/Character.o

INCLUDE_FILES = include/Lato.h include/math.h include/Character.h
C_FILES = src/Lato.c src/math.c src/Character.c

.PHONY: all clean

all: $(LIB_NAME).a

$(LIB_NAME).a: $(OBJ_FILES)
	ar rcs $@ $^

src/lato.o: src/lato.c $(INCLUDE_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

src/math.o: src/math.c $(INCLUDE_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(LIB_NAME).a $(OBJ_FILES)
