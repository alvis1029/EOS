CC = arm-unknown-linux-gnu-gcc -pthread
CXX = arm-unknown-linux-gnu-gxx -pthread
CFLAGS = -g -Wall

LINK = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/
INCLUDE1 = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/
INCLUDE2 = /home/alvisliu/microtime/linux/include/

all: solitare_server solitare_client1 solitare_client2

%.o: %.c
	$(CC) -c $< -L $(LINK) -I $(INCLUDE1) -I $(INCLUDE2) $(CFLAGS)

%.o: %.cpp
	$(CXX) -c $< -L $(LINK) -I $(INCLUDE1) -I $(INCLUDE2) $(CFLAGS)

solitare_server: solitare_server.o
	$(CC) -o $@ $^ $(CFLAGS)

solitare_client1: solitare_client1.o
	$(CC) -o $@ $^ $(CFLAGS)

solitare_client2: solitare_client2.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf solitare_server solitare_server.o
	rm -rf solitare_client1 solitare_client1.o
	rm -rf solitare_client2 solitare_client2.o

.PHONY: all clean
