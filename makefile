CC=gcc
CFLAGS = -o c-sim -ansi -pedantic -Wall -m32 

all: c-sim

c-sim: c-sim.c cache.h
	$(CC) $(CFLAGS) c-sim.c cache.h -I. -lm

clean:
	rm ./c-sim