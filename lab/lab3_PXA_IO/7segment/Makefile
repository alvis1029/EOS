CXX = arm-unknown-linux-gnu-g++
CFLAGS = -g -Wall

LINK = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/
INCLUDE1 = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/
INCLUDE2 = /home/alvisliu/microtime/linux/include/

all: _7seg

%.o: %.c
	$(CXX) -c $< -L $(LINK) -I $(INCLUDE1) -I $(INCLUDE2) $(CFLAGS)

%.o: %.cpp
	$(CXX) -c $< -L $(LINK) -I $(INCLUDE1) -I $(INCLUDE2) $(CFLAGS)

_7seg: _7seg.o
	$(CXX) -o $@ $^ $(CFLAGS)

clean:
	rm -rf _7seg _7seg.o

.PHONY: all clean
