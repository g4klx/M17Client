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

#if !defined(M17Client_H)
#define	M17Client_H

#include "StatusCallback.h"
#include "AudioBackend.h"
#include "AudioCallback.h"
#include "UDPSocket.h"
#if defined(USE_HAMLIB)
#include "HamLib.h"
#endif
#if defined(USE_GPSD)
#include "GPSD.h"
#endif
#if defined(USE_GPIO)
#include "GPIO.h"
#endif
#include "CodePlug.h"
#include "M17RX.h"
#include "M17TX.h"
#include "Conf.h"

#include <string>

class CM17Client : public IAudioCallback, public IStatusCallback
{
public:
	CM17Client(const std::string& confFile);
	virtual ~CM17Client();

	int run();

	virtual void readCallback(const float* input, unsigned int nSamples, int id);
	virtual void writeCallback(float* output, int& nSamples, int id);

	virtual void statusCallback(const std::string& source, const std::string& dest, bool end);
	virtual void textCallback(const char* text);
	virtual void rssiCallback(int rssi);
	virtual void gpsCallback(float latitude, float longitude, const std::string& locator,
			const std::optional<float>& altitude,
			const std::optional<float>& speed, const std::optional<float>& track,
			const std::optional<float>& bearing, const std::optional<float>& distance);
	virtual void callsignsCallback(const char* callsigns);

private:
	CConf            m_conf;
	CCodePlug*       m_codePlug;
	CM17RX*          m_rx;
	CM17TX*          m_tx;
	bool             m_tx1;
	bool             m_tx2;
	IAudioBackend*   m_sound;
	CUDPSocket*      m_socket;
#if defined(USE_HAMLIB)
	CHamLib*         m_hamLib;
#endif
#if defined(USE_GPSD)
	CGPSD*           m_gpsd;
#endif
#if defined(USE_GPIO)
	CGPIO*           m_gpio;
#endif
	sockaddr_storage m_sockaddr;
	unsigned int     m_sockaddrLen;

	void parseCommand(char* command);

	void sendTX(bool tx);

	void sendChannelList();
	void sendDestinationList();

	bool processChannelRequest(const char* channel);
};

#endif

