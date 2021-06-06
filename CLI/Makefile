CC      = cc
CXX     = c++
CFLAGS  = -g -O3 -Wall -std=c++0x -pthread -I/usr/local/include
LIBS    = -lpthread -lutil
LDFLAGS = -g -L/usr/local/lib

OBJECTS = \
		Log.o M17CLI.o Thread.o UDPSocket.o
		
all:		M17CLI

M17CLI:	$(OBJECTS) 
		$(CXX) $(OBJECTS) $(CFLAGS) $(LIBS) -o M17CLI

%.o: %.cpp
		$(CXX) $(CFLAGS) -c -o $@ $<

install:	all
		install -m 755 M17CLI /usr/local/bin/

clean:
		$(RM) M17CLI *.o *.bak *~

