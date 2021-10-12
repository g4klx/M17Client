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


CGPSDialog::CGPSDialog(wxWindow* parent, int id, float latitude, float longitude, const wxString& locator,
			const std::optional<float>& altitude,
			const std::optional<float>& speed, const std::optional<float>& track,
			const std::optional<float>& bearing, const std::optional<float>& distance) :
wxDialog(parent, id, wxString(wxT("GPS Data")), wxDefaultPosition, wxSize(GPSDIALOG_WIDTH, GPSDIALOG_HEIGHT))
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

	CGPSCompass* compass = new CGPSCompass(this, -1, bearing);
	mainSizer->Add(compass, 0, wxALL, BORDER_SIZE);

	wxBoxSizer* textSizer = new wxBoxSizer(wxVERTICAL);

	wxString text;

	if (latitude < 0.0F)
		text.Printf(wxT("Latitude: %.3f\xB0 S"), -latitude);
	else
		text.Printf(wxT("Latitude: %.3f\xB0 N"), latitude);

	wxStaticText* temp = new wxStaticText(this, -1, text);
	textSizer->Add(temp, 0, wxALL, BORDER_SIZE);

	if (longitude < 0.0F)
		text.Printf(wxT("Longitude: %.3f\xB0 W"), -longitude);
	else
		text.Printf(wxT("Longitude: %.3f\xB0 E"), longitude);

	temp = new wxStaticText(this, -1, text);
	textSizer->Add(temp, 0, wxALL, BORDER_SIZE);

	text.Printf(wxT("Locator: %s"), locator.c_str());
	temp = new wxStaticText(this, -1, text);
	textSizer->Add(temp, 0, wxALL, BORDER_SIZE);

	if (altitude) {
		text.Printf(wxT("Altitude: %.1fm"), altitude.value());
		temp = new wxStaticText(this, -1, text);
		textSizer->Add(temp, 0, wxALL, BORDER_SIZE);
	}

	if (speed && track) {
		text.Printf(wxT("Speed: %.1fkm/h"), speed.value());
		temp = new wxStaticText(this, -1, text);
		textSizer->Add(temp, 0, wxALL, BORDER_SIZE);

		text.Printf(wxT("Track: %.0f\xB0"), track.value());
		temp = new wxStaticText(this, -1, text);
		textSizer->Add(temp, 0, wxALL, BORDER_SIZE);
	}

	if (bearing && distance) {
		text.Printf(wxT("Bearing: %.0f\xB0"), bearing.value());
		temp = new wxStaticText(this, -1, text);
		textSizer->Add(temp, 0, wxALL, BORDER_SIZE);

		text.Printf(wxT("Distance: %.0fkm"), distance.value());
		temp = new wxStaticText(this, -1, text);
		textSizer->Add(temp, 0, wxALL, BORDER_SIZE);
	}

	mainSizer->Add(textSizer, 0, wxALL, BORDER_SIZE);

	SetAutoLayout(true);
	Layout();

	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);

	SetSizer(mainSizer);
}

CGPSDialog::~CGPSDialog()
{
}

