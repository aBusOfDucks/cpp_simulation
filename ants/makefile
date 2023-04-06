CC = g++
FLAGS = -Wall -pthread `pkg-config allegro-5 allegro_primitives-5 --libs --cflags`
BIN = ants
SOURCE = ants.cpp 

all:
	$(CC) $(SOURCE) $(FLAGS) -o $(BIN)
