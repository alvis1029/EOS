CXX = arm-unknown-linux-gnu-g++
CFLAGS = -g -Wall

LINK = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/
INCLUDE1 = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/
INCLUDE2 = /home/alvisliu/microtime/linux/include/

all: lcd

%.o: %.c
	$(CXX) -c $< -L $(LINK) -I $(INCLUDE1) -I $(INCLUDE2) $(CFLAGS)

%.o: %.cpp
	$(CXX) -c $< -L $(LINK) -I $(INCLUDE1) -I $(INCLUDE2) $(CFLAGS)

lcd: lcd.o
	$(CXX) -o $@ $^ $(CFLAGS)

clean:
	rm -rf lcd lcd.o

.PHONY: all clean
