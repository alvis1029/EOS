CC = gcc -pthread
CXX = g++ -pthread
CFLAGS = -g -Wall

all: hw2 client

hw2: hw2.o
	$(CC) -o $@ $^ $(CFLAGS)

client: client.o
	$(CXX) -o $@ $^ $(CFLAGS)

clean:
	rm -rf hw2 hw2.o
	rm -rf client client.o

.PHONY: all clean