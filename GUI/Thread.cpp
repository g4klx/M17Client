/*
 *   Copyright (C) 2010-2015,2021 by Jonathan Naylor G4KLX
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

#include "Thread.h"
#include "ReceiveData.h"
#include "App.h"

#include <vector>

const char* DELIMITER = ":";

CThread::CThread(const CConf& conf) :
wxThread(wxTHREAD_JOINABLE),
m_socket(NULL),
m_killed(false)
{
	m_socket = new CUDPReaderWriter(conf.getDaemonAddress(), conf.getDaemonPort(),
					 conf.getSelfAddress(),   conf.getSelfPort());
}

CThread::~CThread()
{
	delete m_socket;
}

void* CThread::Entry()
{
	wxASSERT(m_socket != NULL);

	m_socket->open();

	while (!m_killed) {
		char buffer[1000U];
		int len = m_socket->read(buffer, 1000U);
		if (len > 0) {
			buffer[len] = '\0';

			std::vector<char *> ptrs;

			char* s = buffer;
			char* p;
			while ((p = ::strtok(s, DELIMITER)) != NULL) {
				s = NULL;
				ptrs.push_back(p);
			}

			if (::strcmp(ptrs.at(0U), "CHAN") == 0) {
				wxArrayString channels;
				for (unsigned int i = 1U; i < ptrs.size(); i++) {
					wxString channel = wxString(ptrs.at(i));
					channels.Add(channel);
				}
				::wxGetApp().setChannels(channels);
			} else if (::strcmp(ptrs.at(0U), "DEST") == 0) {
				wxArrayString destinations;
				for (unsigned int i = 1U; i < ptrs.size(); i++) {
					wxString destination = wxString(ptrs.at(i));
					destinations.Add(destination);
				}
				::wxGetApp().setDestinations(destinations);
			} else if (::strcmp(ptrs.at(0U), "RX") == 0) {
				bool end             = std::stoi(ptrs.at(1U)) == 1;
				wxString source      = wxString(ptrs.at(2U));
				wxString destination = wxString(ptrs.at(3U));

				CReceiveData* data = new CReceiveData(source, destination, end);
				::wxGetApp().showReceive(data);
			} else if (::strcmp(ptrs.at(0U), "TEXT") == 0) {
				wxString text = wxString(ptrs.at(1U));
				::wxGetApp().showText(text);
			} else if (::strcmp(ptrs.at(0U), "RSSI") == 0) {
				int rssi = std::stoi(ptrs.at(1U));
				::wxGetApp().showRSSI(rssi);
			}
		}

		Sleep(20UL);
	}

	m_socket->close();

	return NULL;
}

void CThread::kill()
{
	m_killed = true;
}

bool CThread::getChannels()
{
	wxASSERT(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "CHAN");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, "?");

	return m_socket->write(buffer, ::strlen(buffer));
}

bool CThread::setChannel(const wxString& channel)
{
	wxASSERT(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "CHAN");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, channel.ToAscii());

	return m_socket->write(buffer, ::strlen(buffer));
}

bool CThread::getDestinations()
{
	wxASSERT(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "DEST");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, "?");

	return m_socket->write(buffer, ::strlen(buffer));
}

bool CThread::setDestination(const wxString& destination)
{
	wxASSERT(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "DEST");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, destination.ToAscii());

	return m_socket->write(buffer, ::strlen(buffer));
}

bool CThread::setVolume(unsigned int volume)
{
	wxASSERT(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "VOL");
	::strcat(buffer, DELIMITER);
	::sprintf(buffer + ::strlen(buffer), "%u", volume);

	return m_socket->write(buffer, ::strlen(buffer));
}

bool CThread::setMicGain(unsigned int micGain)
{
	wxASSERT(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "MIC");
	::strcat(buffer, DELIMITER);
	::sprintf(buffer + ::strlen(buffer), "%u", micGain);

	return m_socket->write(buffer, ::strlen(buffer));
}

bool CThread::setTransmit(bool transmit)
{
	wxASSERT(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "TX");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, transmit ? "1" : "0");

	return m_socket->write(buffer, ::strlen(buffer));
}

