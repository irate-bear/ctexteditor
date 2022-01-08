CC=gcc
CFLAGS=-g -Wall
LFLAGS=-lncurses
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

OUT=bin/cte

all: $(OUT)

release: CFLAGS=-Wall -O2 -DNDEBUG
release: $(OUT)

$(OUT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LFLAGS)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -r bin/* $(OBJ)/*