CC = gcc
FLAGS = -Wall -g -o

all: stnc

.PHONY : clean
clean:
	rm stnc

stnc: stnc.c partA.c serverB.c clientB.c headers.h
	$(CC) $(FLAGS) stnc stnc.c partA.c serverB.c clientB.c headers.h
