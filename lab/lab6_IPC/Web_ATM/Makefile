CC = gcc -pthread
CXX = g++ -pthread
CFLAGS = -g -Wall

all: lab6 client

lab6: lab6.o
	$(CC) -o $@ $^ $(CFLAGS)

client: client.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf lab6 lab6.o
	rm -rf client client.o

.PHONY: all clean
