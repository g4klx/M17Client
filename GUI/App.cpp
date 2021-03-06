/*
 *   Copyright (C) 2010-2015,2018,2021 by Jonathan Naylor G4KLX
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

wxIMPLEMENT_APP(CApp);

const wxChar* NOLOGGING_SWITCH = wxT("nolog");

CApp::CApp() :
wxApp(),
m_noLog(false),
m_conf(),
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

	wxLogInfo(wxT("Starting ") + APPLICATION_NAME + wxT(" - ") + VERSION);

	// Log the version of wxWidgets and the Operating System
	wxLogInfo(wxT("Using wxWidgets %d.%d.%d on %s"), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER, ::wxGetOsDescription().c_str());

	if (!m_conf.read())
		return false;

	m_frame = new CFrame(m_conf, APPLICATION_NAME + wxT(" - ") + VERSION);
	m_frame->Show();

	SetTopWindow(m_frame);

	createThread(m_conf);

	return wxApp::OnInit();
}

int CApp::OnExit()
{
	wxLogInfo(APPLICATION_NAME + wxT(" is exiting"));

	m_conf.write();

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
	m_frame->setChannels(channels);
}

void CApp::setDestinations(const wxArrayString& destinations) const
{
	m_frame->setDestinations(destinations);
}

void CApp::showTransmit(bool tx) const
{
	m_frame->showTransmit(tx);
}

void CApp::showReceive(CReceiveData* data) const
{
	wxASSERT(data != NULL);
	
	m_frame->showReceive(data);
}

void CApp::showText(const wxString& text) const
{
	m_frame->showText(text);
}

void CApp::showCallsigns(const wxString& callsigns) const
{
	m_frame->showCallsigns(callsigns);
}

void CApp::showRSSI(int rssi) const
{
	m_frame->showRSSI(rssi);
}

void CApp::showGPS(float latitude, float longitude, const wxString& locator,
			const std::optional<float>& altitude,
			const std::optional<float>& speed, const std::optional<float>& track,
			const std::optional<float>& bearing, const std::optional<float>& distance) const
{
	m_frame->showGPS(latitude, longitude, locator, altitude, speed, track, bearing, distance);
}

void CApp::error(const wxString& text) const
{
	m_frame->error(text);
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

bool CApp::setVolume(unsigned int volume)
{
	return m_thread->setVolume(volume);
}

void CApp::createThread(const CConf& conf)
{
	m_thread = new CThread(conf);
	m_thread->Create();
	m_thread->Run();
}

