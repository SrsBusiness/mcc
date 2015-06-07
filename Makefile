VERSION = 0.0.1

CC      = clang
SA      = scan-build

DIR 	:= $(shell pwd)

CFLAGS  = -c -fpic -Wall -Isrc -IcNBT --std=gnu99
NBTFLAGS = -c -Wall -IcNBT -Wextra -std=c99 -pedantic -fPIC
LDFLAGS = -lpthread -lm -lz

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
	$(CC) $(SAMPLE) $(OBJECTS) $(NBTOBJECTS) -o $@ $(LDFLAGS)

# Rules for making all object files
debug: CFLAGS += -g
debug: NBTFLAGS += -g
debug: clean
debug: all

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) $< -o $@

$(NBTOBJ)/%.o: $(NBTSRC)/%.c | $(NBTOBJ)
	$(CC) $(NBTFLAGS) $< -o $@

# Rule for making shared object file
$(SHAREDLIB): $(OBJECTS) $(NBTOBJECTS) | $(LIB)
	$(CC) -shared -o $@ $(OBJECTS) $(NBTOBJECTS)
	ln $(SHAREDLIB) $(LIB)/libmcc.so

.PHONY: test
test: CFLAGS += -g
test: NBTFLAGS += -g
test: $(OBJECTS) $(NBTOBJECTS) $(TEST) | $(BIN)
	$(CC) $(TEST) $(OBJECTS) $(NBTOBJECTS) -o $(BIN)/$@ $(LDFLAGS)
	$(BIN)/$@

.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(NBTOBJECTS) $(TEST) $(SAMPLE) $(BIN)/* $(SHAREDLIB)
	unlink $(LIB)/libmcc.so

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
