/*
 *   Copyright (C) 2010-2014,2018,2020,2021 by Jonathan Naylor G4KLX
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

#ifndef	Conf_H
#define	Conf_H

#include <wx/wx.h>
#include <wx/filename.h>

class CConf {
public:
	CConf();
	~CConf();

	bool read();

	wxString       getDaemonAddress() const;
	unsigned short getDaemonPort() const;

	wxString       getSelfAddress() const;
	unsigned short getSelfPort() const;

	void           setChannel(const wxString& value);
	wxString       getChannel() const;

	void           setDestination(const wxString& value);
	wxString       getDestination() const;

	void           setVolume(unsigned int value);
	unsigned int   getVolume() const;
	
	bool write();

private:
	wxFileName     m_fileName;
	wxString       m_daemonAddress;
	unsigned short m_daemonPort;
	wxString       m_selfAddress;
	unsigned short m_selfPort;
	wxString       m_channel;
	wxString       m_destination;
	unsigned int   m_volume;
};

#endif
