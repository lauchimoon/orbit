CC = gcc
SRC = orbit.c
OUT = orbit
RAYLIB_PATH ?= /home/lauren/software/raylib
LDLIBS = -lm -L/$(RAYLIB_PATH)/src -lraylib -lGL -ldl -lpthread -lX11
INCLUDE = -I/$(RAYLIB_PATH)/src
CFLAGS = -Wall

default:
	$(CC) $(SRC) $(LDLIBS) $(INCLUDE) $(CFLAGS) -o $(OUT)

clean:
	rm $(OUT)
