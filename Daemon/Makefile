# To use HamLib to control the radio, add -DUSE_HAMLIB to the CFLAGS line and add -lhamlib to the LIBS line
#
# To use GPSD to transmit GPS data, add -DUSE_GPSD to the CFLAGS line and add -lgps to the LIBS line
#
# To use GPIO for PTT, add -DUSE_GPIO to the CFLAGS line and add -lgpiod to the LIBS line
#

CC      = cc
CXX     = c++
CFLAGS  = -g -O3 -Wall -std=c++17 -pthread
LIBS    = -lpthread -lutil -lsamplerate
LDFLAGS = -g

AUDIO  ?= alsa

OBJECTS = \
		codec2/codebooks.o codec2/codec2.o codec2/kiss_fft.o codec2/lpc.o codec2/nlp.o codec2/pack.o codec2/qbase.o \
		codec2/quantise.o CodePlug.o Conf.o Golay24128.o GPIO.o GPSD.o HamLib.o Log.o M17Client.o M17Convolution.o \
		M17CRC.o M17LSF.o M17RX.o M17TX.o M17Utils.o Modem.o ModemPort.o RSSIInterpolator.o StopWatch.o Thread.o \
		Timer.o UARTController.o UDPSocket.o Utils.o

ifeq ($(filter $(AUDIO), alsa pulse),)
$(error error: supported audio backends: alsa, pulse)
endif

ifeq ($(AUDIO), alsa)
LIBS    += -lasound
OBJECTS += SoundALSA.o
endif

ifeq ($(AUDIO), pulse)
CFLAGS  += -DUSE_PULSEAUDIO
LIBS    += -lpulse-simple
OBJECTS += SoundPulse.o
endif

all:		M17Client

M17Client:	GitVersion.h $(OBJECTS) 
		$(CXX) $(OBJECTS) $(CFLAGS) $(LIBS) -o M17Client

%.o: %.cpp
		$(CXX) $(CFLAGS) -c -o $@ $<

install:	all
		install -m 755 M17Client /usr/local/bin/

clean:
		$(RM) M17Client codec2/*.o codec2/*.bak codec2/*~ *.o *.bak *~ GitVersion.h

GitVersion.h:
	echo "const char *gitversion = \"$(shell git rev-parse HEAD)\";" > $@

