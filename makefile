CC = gcc
FLAGS = -Wall -g -o

all: stnc

.PHONY : clean
clean:
	rm stnc *.txt

stnc: stnc.c partA.c serverB.c clientB.c headers.h
	$(CC) $(FLAGS) stnc stnc.c partA.c serverB.c clientB.c headers.h
