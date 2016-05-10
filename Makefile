CC = gcc

SRC_M = master.c
SRC_S = slave.c

OBJ_M = master
OBJ_S = slave

all:
	gcc -Wall -o $(OBJ_M) $(SRC_M)
	gcc -Wall -o $(OBJ_S) $(SRC_S)


