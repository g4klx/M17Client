/*
 *   Copyright (C) 2010,2011,2012,2014,2021 by Jonathan Naylor G4KLX
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

#ifndef	Frame_H
#define	Frame_H

#include "ReceiveData.h"
#include "Defs.h"

#include <wx/wx.h>
#include <wx/tglbtn.h>
#include <wx/listctrl.h>

class CFrame : public wxFrame {
public:
	CFrame(const wxString& title);
	virtual ~CFrame();

	virtual void onQuit(wxCommandEvent& event);

	virtual void onAbout(wxCommandEvent& event);

	virtual void onClose(wxCloseEvent& event);

	virtual void onChannel(wxCommandEvent& event);

	virtual void onTransmit(wxCommandEvent& event);

	virtual void onVolume(wxScrollEvent& event);
	virtual void onMicGain(wxScrollEvent& event);

	virtual void onTimer(wxTimerEvent& event);

	virtual void setChannels(const wxArrayString& channels);
	virtual void setDestinations(const wxArrayString& channels);

	virtual void showReceive(CReceiveData* data);
	virtual void showRSSI(int rssi);
	virtual void showText(const wxString& text);
	virtual void error(const wxString& error);

	virtual void onChannels(wxEvent& event);
	virtual void onDestinations(wxEvent& event);

	virtual void onReceive(wxEvent& event);
	virtual void onRSSI(wxEvent& event);
	virtual void onText(wxEvent& event);
	virtual void onError(wxEvent& event);

private:
	wxChoice*       m_channels;
	wxChoice*       m_destinations;
	wxChoice*       m_modules;
	wxToggleButton* m_transmit;
	wxStaticText*   m_status;
	wxSlider*       m_volume;
	wxSlider*       m_micGain;
	wxStaticText*   m_hrdSource;
	wxStaticText*   m_hrdDestination;
	wxStaticText*   m_hrdText;
	wxStaticText*   m_hrdRSSI;
	wxListCtrl*     m_heard;
	wxTimer         m_timer;

	DECLARE_EVENT_TABLE()

	wxMenuBar* createMenuBar();
};

#endif
