CC = gcc
CXX = g++
CFLAGS = -g -Wall

all: lab5

lab5: lab5.o
	$(CXX) -o $@ $^ $(CFLAGS)

clean:
	rm -rf lab5 lab5.o

.PHONY: all clean
