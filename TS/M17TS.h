/*
 *   Copyright (C) 2015,2016,2017,2019,2020,2021 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#if !defined(M17TS_H)
#define	M17TS_H

#include "UARTController.h"
#include "UDPSocket.h"
#include "Conf.h"

#include <vector>
#include <string>

class CM17TS
{
public:
	CM17TS();
	virtual ~CM17TS();

	int run();

private:
	CConf            m_conf;
	CUDPSocket*      m_socket;
	CUARTController* m_uart;
	sockaddr_storage m_sockaddr;
	unsigned int     m_sockaddrLen;

	std::vector<std::string> m_channels;
	std::vector<std::string> m_destinations;
	std::vector<char>        m_modules;

	unsigned int m_channelIdx;
	unsigned int m_destinationIdx;
	unsigned int m_moduleIdx;
	
	bool         m_transmit;
	
	void parseCommand(char* command);
	void parseScreen(char* command);

	void channelUp();
	void channelDown();
	void destinationUp();
	void destinationDown();
	void moduleUp();
	void moduleDown();

	void transmit();
	
	void page0Next();
	void page1Next();

	bool getChannels();
	bool setChannel(const std::string& channel);
	bool getDestinations();
	bool setDestination(const std::string& destination);
	bool setVolume(unsigned int volume);
	bool setMicGain(unsigned int micGain);
	bool setTransmit(bool transmit);

	void sendCommand(const char* command);
	
	void selectChannel();
	void selectDestination();
	void selectModule();
};

#endif

