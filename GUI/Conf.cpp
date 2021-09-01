/*
 *   Copyright (C) 2010-2015,2018,2020,2021 by Jonathan Naylor G4KLX
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

#include "Conf.h"

#include <wx/textfile.h>

const wxString  KEY_DAEMON_ADDRESS = wxT("DaemonAddress");
const wxString  KEY_DAEMON_PORT    = wxT("DaemonPort");
const wxString  KEY_SELF_ADDRESS   = wxT("SelfAddress");
const wxString  KEY_SELF_PORT      = wxT("SelfPort");
const wxString  KEY_CHANNEL        = wxT("Channel");
const wxString  KEY_DESTINATION    = wxT("Destination");
const wxString  KEY_VOLUME         = wxT("Volume");
const wxString  KEY_MIC_GAIN       = wxT("MicGain");


CConf::CConf() :
m_fileName(),
m_daemonAddress(wxT("127.0.0.1")),
m_daemonPort(7658U),
m_selfAddress(wxT("127.0.0.1")),
m_selfPort(7659U),
m_channel(),
m_destination(),
m_volume(100U)
{
	m_fileName.AssignHomeDir();
	m_fileName.SetFullName(wxT(".M17GUI"));
}

CConf::~CConf()
{
}

bool CConf::read()
{
	wxASSERT(!m_fileName.GetFullPath().IsEmpty());

	wxTextFile file(m_fileName.GetFullPath());

	bool exists = file.Exists();
	if (!exists)
		return true;

	bool ret = file.Open();
	if (!ret) {
		wxLogError(wxT("Cannot open the config file - %s"), m_fileName.GetFullPath().c_str());
		return false;
	}

	unsigned long temp;

	wxString str = file.GetFirstLine();

	while (!file.Eof()) {
		if (str.GetChar(0U) == wxT('#')) {
			str = file.GetNextLine();
			continue;
		}

		int n = str.Find(wxT('='));
		if (n == wxNOT_FOUND) {
			str = file.GetNextLine();
			continue;
		}

		wxString key = str.Left(n);
		wxString val = str.Mid(n + 1U);

		if (key.IsSameAs(KEY_DAEMON_ADDRESS)) {
			m_daemonAddress = val;
		} else if (key.IsSameAs(KEY_DAEMON_PORT)) {
			val.ToULong(&temp);
			m_daemonPort = (unsigned short)temp;
		} else if (key.IsSameAs(KEY_SELF_ADDRESS)) {
			m_selfAddress = val;
		} else if (key.IsSameAs(KEY_SELF_PORT)) {
			val.ToULong(&temp);
			m_selfPort = (unsigned short)temp;
		} else if (key.IsSameAs(KEY_CHANNEL)) {
			m_channel = val;
		} else if (key.IsSameAs(KEY_DESTINATION)) {
			m_destination = val;
		} else if (key.IsSameAs(KEY_VOLUME)) {
			val.ToULong(&temp);
			m_volume = (unsigned int)temp;
		}

		str = file.GetNextLine();
	}

	file.Close();

	return true;
}

wxString CConf::getDaemonAddress() const
{
	return m_daemonAddress;
}

unsigned short CConf::getDaemonPort() const
{
	return m_daemonPort;
}

wxString CConf::getSelfAddress() const
{
	return m_selfAddress;
}

unsigned short CConf::getSelfPort() const
{
	return m_selfPort;
}

wxString CConf::getChannel() const
{
	return m_channel;
}

void CConf::setChannel(const wxString& value)
{
	m_channel = value;
}

wxString CConf::getDestination() const
{
	return m_destination;
}

void CConf::setDestination(const wxString& value)
{
	m_destination = value;
}

unsigned int CConf::getVolume() const
{
	return m_volume;
}

void CConf::setVolume(unsigned int value)
{
	m_volume = value;
}

bool CConf::write()
{
	wxTextFile file(m_fileName.GetFullPath());

	bool exists = file.Exists();
	if (exists) {
		bool ret = file.Open();
		if (!ret) {
			wxLogError(wxT("Cannot open the config file - %s"), m_fileName.GetFullPath().c_str());
			return false;
		}

		// Remove the existing file entries
		file.Clear();
	} else {
		bool ret = file.Create();
		if (!ret) {
			wxLogError(wxT("Cannot create the config file - %s"), m_fileName.GetFullPath().c_str());
			return false;
		}
	}

	wxString buffer;

	buffer.Printf(wxT("%s=%s"), KEY_DAEMON_ADDRESS.c_str(), m_daemonAddress.c_str()); file.AddLine(buffer);
	buffer.Printf(wxT("%s=%u"), KEY_DAEMON_PORT.c_str(), m_daemonPort); file.AddLine(buffer);

	buffer.Printf(wxT("%s=%s"), KEY_SELF_ADDRESS.c_str(), m_selfAddress.c_str()); file.AddLine(buffer);
	buffer.Printf(wxT("%s=%u"), KEY_SELF_PORT.c_str(), m_selfPort); file.AddLine(buffer);

	buffer.Printf(wxT("%s=%s"), KEY_CHANNEL.c_str(), m_channel.c_str()); file.AddLine(buffer);
	buffer.Printf(wxT("%s=%s"), KEY_DESTINATION.c_str(), m_destination.c_str()); file.AddLine(buffer);

	buffer.Printf(wxT("%s=%u"), KEY_VOLUME.c_str(), m_volume); file.AddLine(buffer);

	bool ret = file.Write();
	if (!ret) {
		file.Close();
		wxLogError(wxT("Cannot write the config file - %s"), m_fileName.GetFullPath().c_str());
		return false;
	}

	file.Close();

	return true;
}

