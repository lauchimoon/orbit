CC = gcc
SRC = orbit.c
OUT = orbit
LDLIBS = -lm -L/home/lauren/software/raylib/src -lraylib -lGL -ldl -lpthread -lX11
INCLUDE = -I/home/lauren/software/raylib/src
CFLAGS = -Wall

default:
	$(CC) $(SRC) $(LDLIBS) $(INCLUDE) $(CFLAGS) -o $(OUT)

clean:
	rm $(OUT)
