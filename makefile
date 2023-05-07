CC = gcc
FLAGS = -Wall -g -o

all: stnc

.PHONY : clean
clean:
	rm stnc

stnc: partA.c
	$(CC) $(FLAGS) stnc partA.c