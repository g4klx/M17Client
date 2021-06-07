export BINDIR  := /usr/bin

export CXX     := $(shell wx-config --cxx)
export CFLAGS  := -g -O2 -Wall $(shell wx-config --cxxflags)
export LIBS    := $(shell wx-config --libs adv,core) -lasound -lusb-1.0
export LDFLAGS := -g

OBJECTS =	App.o ChannelsEvent.o DestinationsEvent.o ErrorEvent.o Frame.o Logger.o ReceiveData.o ReceiveEvent.o \
		RSSIEvent.o TextEvent.o Thread.o UDPReaderWriter.o Utils.o

all:		M17GUI

M17GUI:	$(OBJECTS)
		$(CXX) $(OBJECTS) $(LDFLAGS) $(LIBS) -o M17GUI

%.o: %.cpp
		$(CXX) $(CFLAGS) -c -o $@ $<

install:
		install -g bin -o root -m 0775 M17GUI $(BINDIR)

clean:
		$(RM) M17GUI *.o *.bak *~

