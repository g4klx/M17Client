export BINDIR  := /usr/bin

export CXX     := $(shell wx-config --cxx)
export CFLAGS  := -g -O2 -std=c++17 -Wall $(shell wx-config --cxxflags)
export LIBS    := $(shell wx-config --libs adv,core)
export LDFLAGS := -g

OBJECTS =	App.o ChannelsEvent.o CallsignsEvent.o Conf.o DestinationsEvent.o ErrorEvent.o Frame.o GPSCompass.o GPSDialog.o \
		GPSEvent.o Logger.o ReceiveData.o ReceiveEvent.o RSSIEvent.o TextEvent.o Thread.o TransmitEvent.o UDPReaderWriter.o \
		Utils.o

all:		M17GUI

M17GUI:	$(OBJECTS)
		$(CXX) $(OBJECTS) $(LDFLAGS) $(LIBS) -o M17GUI

%.o: %.cpp
		$(CXX) $(CFLAGS) -c -o $@ $<

install:
		install -g bin -o root -m 0775 M17GUI $(BINDIR)

clean:
		$(RM) M17GUI *.o *.bak *~

