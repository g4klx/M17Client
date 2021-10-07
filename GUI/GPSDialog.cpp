/*
 *   Copyright (C) 2021 by Jonathan Naylor G4KLX
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

#include "GPSDialog.h"
#include "GPSCompass.h"
#include "Defs.h"

BEGIN_EVENT_TABLE(CGPSDialog, wxDialog)
	EVT_BUTTON(wxID_OK, CGPSDialog::onOK)
END_EVENT_TABLE()

CGPSDialog::CGPSDialog(wxWindow* parent, int id, float latitude, float longitude, const wxString& locator,
			const std::optional<float>& altitude,
			const std::optional<float>& speed, const std::optional<float>& track,
			const std::optional<float>& bearing, const std::optional<float>& distance) :
wxDialog(parent, id, wxString(_("GPS Data")))
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

	CGPSCompass* compass = new CGPSCompass(this, -1, bearing, wxDefaultPosition, wxSize(COMPASS_WIDTH, COMPASS_HEIGHT));
	mainSizer->Add(compass, 0, wxALL, BORDER_SIZE);

	wxBoxSizer* textSizer = new wxBoxSizer(wxVERTICAL);

	wxString text;

	if (latitude < 0.0F)
		text.Printf(wxT("%f\u2103 S"), -latitude);
	else
		text.Printf(wxT("%f\u2103 N"), latitude);

	wxStaticText* temp = new wxStaticText(this, -1, text);
	textSizer->Add(temp, 0, wxALL, BORDER_SIZE);

	if (longitude < 0.0F)
		text.Printf(wxT("%f\u2103 W"), -longitude);
	else
		text.Printf(wxT("%f\u2103 E"), longitude);

	temp = new wxStaticText(this, -1, text);
	textSizer->Add(temp, 0, wxALL, BORDER_SIZE);

	temp = new wxStaticText(this, -1, locator);
	textSizer->Add(temp, 0, wxALL, BORDER_SIZE);

	if (altitude) {
		text.Printf(wxT("%fm"), altitude.value());
		temp = new wxStaticText(this, -1, text);
		textSizer->Add(temp, 0, wxALL, BORDER_SIZE);
	}

	if (speed && track) {
		text.Printf(wxT("%fkm/h"), speed.value());
		temp = new wxStaticText(this, -1, text);
		textSizer->Add(temp, 0, wxALL, BORDER_SIZE);

		text.Printf(wxT("%f\u2103"), track.value());
		temp = new wxStaticText(this, -1, text);
		textSizer->Add(temp, 0, wxALL, BORDER_SIZE);
	}

	if (bearing && distance) {
		text.Printf(wxT("%f\u2103"), bearing.value());
		temp = new wxStaticText(this, -1, text);
		textSizer->Add(temp, 0, wxALL, BORDER_SIZE);

		text.Printf(wxT("%fkm"), distance.value());
		temp = new wxStaticText(this, -1, text);
		textSizer->Add(temp, 0, wxALL, BORDER_SIZE);
	}

	mainSizer->Add(textSizer, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(CreateButtonSizer(wxOK), 0, wxALL, BORDER_SIZE);

	SetAutoLayout(true);
	Layout();

	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);

	SetSizer(mainSizer);
}

CGPSDialog::~CGPSDialog()
{
}

void CGPSDialog::onOK(wxCommandEvent& WXUNUSED(event))
{
	if (IsModal()) {
		EndModal(wxID_OK);
	} else {
		SetReturnCode(wxID_OK);
		Show(false);
	}
}

