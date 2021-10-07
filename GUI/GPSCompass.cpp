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

#include "GPSCompass.h"

#include <cmath>

#define	DEG2RAD(x)	((x / 180.0F) * 3.14159F)
#define	RAD2DEG(x)	((x / 3.14159F) * 180.0F)

BEGIN_EVENT_TABLE(CGPSCompass, wxPanel)
	EVT_PAINT(CGPSCompass::onPaint)
END_EVENT_TABLE()

CGPSCompass::CGPSCompass(wxWindow* parent, int id, const std::optional<float>& bearing, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
wxPanel(parent, id, pos, size, style, name),
m_width(size.GetWidth()),
m_height(size.GetHeight()),
m_background(NULL),
m_bitmap(NULL)
{
	m_bitmap     = new wxBitmap(m_width, m_height);
	m_background = new wxBitmap(m_width, m_height);

	createBackground();

	if (bearing)
		setPointer(bearing.value());
}

CGPSCompass::~CGPSCompass()
{
	delete m_bitmap;
	delete m_background;
}

void CGPSCompass::setPointer(float bearing)
{
	wxMemoryDC dcBackground;
	dcBackground.SelectObject(*m_background);

	wxMemoryDC dc;
	dc.SelectObject(*m_bitmap);

	dc.Blit(0, 0, m_width, m_height, &dcBackground, 0, 0);
	dcBackground.SelectObject(wxNullBitmap);

	// Draw the lines
	dc.SetBrush(*wxYELLOW_BRUSH);
	dc.SetPen(*wxYELLOW_PEN);

	int needleRadius = m_width / 2 - 30;

	int centreX = m_width / 2;
	int centreY = m_height / 2;

	int p1x = centreX + needleRadius * ::cos(DEG2RAD(bearing));
	int p1y = centreY + needleRadius * ::sin(DEG2RAD(bearing));

	int p2x = centreX + needleRadius * ::cos(DEG2RAD(bearing + 145.0f));
	int p2y = centreY + needleRadius * ::sin(DEG2RAD(bearing + 145.0f));

	int p3x = centreX + needleRadius / 2 * ::cos(DEG2RAD(bearing + 180.0f));
	int p3y = centreY + needleRadius / 2 * ::sin(DEG2RAD(bearing + 180.0f));

	int p4x = centreX + needleRadius * ::cos(DEG2RAD(bearing - 145.0f));
	int p4y = centreY + needleRadius * ::sin(DEG2RAD(bearing - 145.0f));

	dc.DrawLine(p1x, p1y, p2x, p2y);
	dc.DrawLine(p2x, p2y, p3x, p3y);
	dc.DrawLine(p3x, p3y, p4x, p4y);
	dc.DrawLine(p4x, p4y, p1x, p1y);

	dc.SelectObject(wxNullBitmap);

	wxClientDC clientDC(this);
	show(clientDC);
}

void CGPSCompass::onPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC dc(this);

	show(dc);
}

void CGPSCompass::show(wxDC& dc)
{
	dc.DrawBitmap(*m_bitmap, 0, 0, false);
}

void CGPSCompass::createBackground()
{
	// Flood the graph area with black to start with
	wxMemoryDC dc;

	dc.SelectObject(*m_background);

	dc.SetBackground(*wxBLACK_BRUSH);
	dc.Clear();

	dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetPen(*wxWHITE_PEN);
	dc.DrawCircle(m_width / 2, m_height / 2, m_width / 2 - 20);

	int x = m_width / 2 - 10;
	int y = m_height - 20;
	dc.DrawRectangle(x, y, 20, 20);

	dc.SetPen(*wxBLACK_PEN);

	wxFont font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
	font.SetPointSize(11);
	font.SetWeight(wxFONTWEIGHT_BOLD);

	dc.SetFont(font);

	dc.SetTextForeground(*wxBLACK);

	// North
	x = m_width / 2 - 3;
	y = m_height - 10;
	dc.DrawText(wxT("N"), x, y);

	dc.SelectObject(wxNullBitmap);
}

