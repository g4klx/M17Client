/*
 *   Copyright (C) 2010-2015,2018 by Jonathan Naylor G4KLX
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

#include "App.h"
#include "Version.h"
#include "Logger.h"

#include <wx/cmdline.h>
#include <wx/tokenzr.h>
#include <wx/config.h>
#include <wx/filename.h>
#include <boost/bind.hpp>

wxIMPLEMENT_APP(CApp);

const wxChar* NOLOGGING_SWITCH = wxT("nolog");

CApp::CApp() :
wxApp(),
m_noLog(false),
m_frame(NULL),
m_thread(NULL)
{
}

CApp::~CApp()
{
}

bool CApp::OnInit()
{
	SetVendorName(VENDOR_NAME);

	if (!wxApp::OnInit())
		return false;

	if (!m_noLog) {
		wxLog* log = new CLogger(wxFileName::GetHomeDir(), LOG_BASE_NAME);
		wxLog::SetActiveTarget(log);
	} else {
		new wxLogNull;
	}

	m_frame = new CFrame(APPLICATION_NAME + wxT(" - ") + VERSION);
	m_frame->Show();

	SetTopWindow(m_frame);

	wxLogInfo(wxT("Starting ") + APPLICATION_NAME + wxT(" - ") + VERSION);

	// Log the version of wxWidgets and the Operating System
	wxLogInfo(wxT("Using wxWidgets %d.%d.%d on %s"), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER, ::wxGetOsDescription().c_str());

	createThread();

	return wxApp::OnInit();
}

int CApp::OnExit()
{
	wxLogInfo(APPLICATION_NAME + wxT(" is exiting"));

	m_thread->kill();
	m_thread->Wait();

	return 0;
}

void CApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	parser.AddSwitch(NOLOGGING_SWITCH, wxEmptyString, wxEmptyString, wxCMD_LINE_PARAM_OPTIONAL);

	wxApp::OnInitCmdLine(parser);
}

bool CApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	if (!wxApp::OnCmdLineParsed(parser))
		return false;

	m_noLog = parser.Found(NOLOGGING_SWITCH);

	return true;
}

#if defined(__WXDEBUG__)
void CApp::OnAssertFailure(const wxChar* file, int line, const wxChar* func, const wxChar* cond, const wxChar* msg)
{
	wxLogFatalError(wxT("Assertion failed on line %d in file %s and function %s: %s %s"), line, file, func, cond, msg);
}
#endif


void CApp::setChannels(const wxArrayString& channels) const
{
	m_frame->GetEventHandler()->CallAfter(boost::bind(&CFrame::setChannels, m_frame, channels));
}

void CApp::setDestinations(const wxArrayString& destinations) const
{
	m_frame->GetEventHandler()->CallAfter(boost::bind(&CFrame::setDestinations, m_frame, destinations));
}

void CApp::showReceive(CReceiveData* data) const
{
	wxASSERT(data != NULL);
	
	m_frame->GetEventHandler()->CallAfter(boost::bind(&CFrame::showReceive, m_frame, data));
}

void CApp::showText(const wxString& text) const
{
	m_frame->GetEventHandler()->CallAfter(boost::bind(&CFrame::showText, m_frame, text));
}

void CApp::showRSSI(int rssi) const
{
	m_frame->GetEventHandler()->CallAfter(boost::bind(&CFrame::showRSSI, m_frame, rssi));
}

void CApp::error(const wxString& text) const
{
	m_frame->GetEventHandler()->CallAfter(boost::bind(&CFrame::error, m_frame, text));
}

bool CApp::getChannels()
{
	return m_thread->getChannels();
}

bool CApp::getDestinations()
{
	return m_thread->getDestinations();
}

bool CApp::setChannel(const wxString& channel)
{
	return m_thread->setChannel(channel);
}

bool CApp::setDestination(const wxString& destination)
{
	return m_thread->setDestination(destination);
}

bool CApp::setTransmit(bool on)
{
	return m_thread->setTransmit(on);
}

void CApp::createThread()
{
	m_thread = new CThread;
	m_thread->Create();
	m_thread->Run();
}

