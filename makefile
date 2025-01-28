CC = clang
CFLAGS = -Wall -std=c99 -O2
LDFLAGS = -lraylib -lm 

SRC = $(wildcard src/*.c)
OUT = raylib_project

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)

