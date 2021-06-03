CC = gcc
CXX = g++
CFLAGS = -g -Wall

all: game guess

game: game.o
	$(CXX) -o $@ $^ $(CFLAGS)

guess: guess.o
	$(CXX) -o $@ $^ $(CFLAGS)

clean:
	rm -rf game game.o
	rm -rf guess guess.o

.PHONY: all clean