CC = gcc -pthread
CXX = g++ -pthread
CFLAGS = -g -Wall

all: hw3 client

hw3: hw3.o
	$(CC) -o $@ $^ $(CFLAGS)

client: client.o
	$(CXX) -o $@ $^ $(CFLAGS)

clean:
	rm -rf hw3_ho hw3_ho.o
	rm -rf hw3 hw3.o
	rm -rf client client.o

.PHONY: all clean