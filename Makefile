VERSION = 0.0.1

CC      = clang
SA      = scan-build

DIR 	:= $(pwd)

CFLAGS  	= $(shell cat .cflags)
LDFLAGS 	= -lpthread -lm -luv -lz -lssl -lcrypto -lcurl -ljson-c

SRC		= src
LIB		= lib
SHAREDLIB	= $(LIB)/libmcc.so.$(VERSION)
OBJ		= obj
BIN		= bin
TARGET  = $(BIN)/mcc
SRCFILES	:=	\
	auth.c		\
	bot.c		\
	nbt.c		\
	protocol.c	\
	serial.c	\
	
OBJECTS 	:= $(patsubst %.c,$(OBJ)/%.o, $(SRCFILES))

all: $(TARGET)

$(TARGET): $(SHAREDLIB) | $(BIN)
	$(CC) $(OBJECTS) $(SRC)/test.c -o $@ $(LDFLAGS)

test: $(SHAREDLIB) | $(BIN)
	$(CC) $(OBJECTS) $(SRC)/test.c -o test $(LDFLAGS)

debug: CFLAGS += -g
debug: clean
debug: all


# Rule for making all object files
$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) -c -fpic $(CFLAGS) $< -o $@

# Rule for making shared object file
$(SHAREDLIB): $(OBJECTS) | $(LIB)
	$(CC) -shared -o $@ $(OBJECTS) $(LDFLAGS)


.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(BIN)/* $(SHAREDLIB)

.PHONY: %.lint
%.lint: %
	$(CC) -fsyntax-only $(CFLAGS) $^

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
