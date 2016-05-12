CC = gcc

SRC_M = utils.c master.c 
SRC_S = utils.c slave.c

OBJ_M = master
OBJ_S = slave

all:
	gcc -Wall -o $(OBJ_M) $(SRC_M)
	gcc -Wall -o $(OBJ_S) $(SRC_S)


