VERSION = 0.0.1

CC      = gcc
SA      = scan-build

DIR 	:= $(pwd)

CFLAGS  = -c -fpic -Wall -Isrc --std=gnu99
NBTFLAGS = -c -Wall -IcNBT -Wextra -std=c99 -pedantic -fPIC
LDFLAGS = -lpthread -lm

SRC		= src
NBTSRC	= cNBT
LIB		= lib
SHAREDLIB	= $(LIB)/libmcc.so.$(VERSION)
OBJ		= obj
NBTOBJ 	= $(OBJ)/cNBT
BIN		= bin
TARGET  = $(BIN)/mcc

NBTSRCFILES	= $(NBTSRC)/buffer.c $(NBTSRC)/nbt_loading.c $(NBTSRC)/nbt_parsing.c $(NBTSRC)/nbt_treeops.c $(NBTSRC)/nbt_util.c

TEST    := $(patsubst %.c,%.o,$(wildcard test/*.c))
SRCFILES	:= $(wildcard $(SRC)/*.c)
OBJECTS 	:= $(patsubst $(SRC)/%.c,$(OBJ)/%.o, $(SRCFILES))
NBTOBJECTS	:= $(patsubst $(NBTSRC)/%.c,$(NBTOBJ)/%.o, $(NBTSRCFILES))
SAMPLE		:= $(patsubst %.c,%.o,$(wildcard sample/*.c))

all: $(TARGET)

$(TARGET): $(SHAREDLIB) $(SAMPLE) | $(BIN)
	$(CC) $(LDFLAGS) $(OBJECTS) $(SAMPLE) -o $@

# Rules for making all object files
$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) $< -o $@

$(NBTOBJ)/%.o: $(NBTSRC)/%.c | $(NBTOBJ)
	$(CC) $(NBTFLAGS) $< -o $@

# Rule for making shared object file
$(SHAREDLIB): $(OBJECTS) $(NBTOBJECTS) | $(LIB)
	$(CC) -shared -o $@ $(OBJECTS) $(NBTOBJECTS)

.PHONY: tests
tests: $(SHAREDLIB) $(TEST) | $(BIN)
	$(CC) $(LDFLAGS) $(OBJECTS) $(TEST) -o bin/$@
	bin/$@

.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(NBTOBJECTS) $(TEST) $(SAMPLE) $(BIN)/* $(SHAREDLIB)

$(NBTOBJ): | $(OBJ)
	mkdir -p $@

$(OBJ):
	mkdir -p $@

$(LIB):
	mkdir -p $@

$(BIN):
	mkdir -p $@

.PHONY: scan
scan:
	$(SA) make all

.PHONY: format
format:
	astyle -n -r --style=linux *.c *.h
