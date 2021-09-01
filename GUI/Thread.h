/*
 *   Copyright (C) 2010,2011,2012,2014,2015,2021 by Jonathan Naylor G4KLX
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

#ifndef	Thread_H
#define	Thread_H

#include "UDPReaderWriter.h"
#include "Conf.h"

#include <wx/wx.h>

class CThread : public wxThread {
public:
	CThread(const CConf& conf);
	virtual ~CThread();

	virtual bool getChannels();
	virtual bool getDestinations();

	virtual bool setChannel(const wxString& channel);
	virtual bool setDestination(const wxString& destination);
	virtual bool setTransmit(bool transmit);

	virtual bool setVolume(unsigned int volume);

	virtual void* Entry();
	virtual void  kill();

private:
	CUDPReaderWriter* m_socket;
	bool              m_killed;
};

#endif
