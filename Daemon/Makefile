CC      = cc
CXX     = c++
CFLAGS  = -g -O3 -Wall -std=c++0x -pthread -I/usr/local/include
LIBS    = -lpthread -lutil -lasound
LDFLAGS = -g -L/usr/local/lib

OBJECTS = \
		CodePlug.o Conf.o Golay24128.o Log.o M17Convolution.o M17CRC.o M17LSF.o M17RX.o M17TX.o M17Utils.o \
		Modem.o ModemPort.o RSSIInterpolator.o SoundCard.o \
		StopWatch.o Thread.o Timer.o UARTController.o UDPSocket.o Utils.o
		
all:		Daemon

Daemon:	GitVersion.h $(OBJECTS) 
		$(CXX) $(OBJECTS) $(CFLAGS) $(LIBS) -o Daemon

%.o: %.cpp
		$(CXX) $(CFLAGS) -c -o $@ $<

install:	all
		install -m 755 Daemon /usr/local/bin/

clean:
		$(RM) Daemon *.o *.d *.bak *~ GitVersion.h

# Export the current git version if the index file exists, else 000...
GitVersion.h:
ifneq ("$(wildcard .git/index)","")
	echo "const char *gitversion = \"$(shell git rev-parse HEAD)\";" > $@
else
	echo "const char *gitversion = \"0000000000000000000000000000000000000000\";" > $@
endif
