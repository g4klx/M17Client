/*
 *   Copyright (C) 2010,2011,2012,2014,2015,2018,2021 by Jonathan Naylor G4KLX
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

#ifndef	App_H
#define	App_H

#include "ReceiveData.h"
#include "Thread.h"
#include "Frame.h"
#include "Conf.h"
#include "Defs.h"

#include <wx/wx.h>

class CApp : public wxApp {

public:
	CApp();
	virtual ~CApp();

	virtual bool OnInit();
	virtual int  OnExit();

	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

	// This is overridden because dialog boxes from threads are bad news
#if defined(__WXDEBUG__)
	virtual void OnAssertFailure(const wxChar* file, int line, const wxChar* func, const wxChar* cond, const wxChar* msg);
#endif
	virtual void showTransmit(bool tx) const;
	virtual void showReceive(CReceiveData* data) const;
	virtual void showText(const wxString& text) const;
	virtual void showRSSI(int rssi) const;

	virtual bool getChannels();
	virtual bool getDestinations();

	virtual void setChannels(const wxArrayString& channels) const;
	virtual void setDestinations(const wxArrayString& destinations) const;

	virtual bool setChannel(const wxString& channel);
	virtual bool setDestination(const wxString& destination);
	virtual bool setTransmit(bool on);

	virtual bool setVolume(unsigned int volume);
	virtual bool setMicGain(unsigned int micGain);

	virtual void error(const wxString& text) const;

private:
	bool     m_noLog;
	CConf    m_conf;
	CFrame*  m_frame;
	CThread* m_thread;

	void createThread(const CConf& conf);
};

wxDECLARE_APP(CApp);

#endif

