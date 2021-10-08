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
#include "Defs.h"

#include <cmath>

#define	DEG2RAD(x)	((x / 180.0F) * 3.14159F)
#define	RAD2DEG(x)	((x / 3.14159F) * 180.0F)

const int WIDTH  = COMPASS_RADIUS * 2 + 30;
const int HEIGHT = COMPASS_RADIUS * 2 + 30;

BEGIN_EVENT_TABLE(CGPSCompass, wxPanel)
	EVT_PAINT(CGPSCompass::onPaint)
END_EVENT_TABLE()

CGPSCompass::CGPSCompass(wxWindow* parent, int id, const std::optional<float>& bearing) :
wxPanel(parent, id, wxDefaultPosition, wxSize(WIDTH, HEIGHT), 0L, "GPS Compass"),
m_background(NULL),
m_bitmap(NULL)
{
	m_bitmap     = new wxBitmap(WIDTH, HEIGHT);
	m_background = new wxBitmap(WIDTH, HEIGHT);

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

	const int needleRadius = COMPASS_RADIUS - 10;

	bearing -= 90.0F;

	float degrees = bearing;
	float radians = DEG2RAD(degrees);
	int p1x = (WIDTH / 2)  + needleRadius * ::cos(radians);
	int p1y = (HEIGHT / 2) + needleRadius * ::sin(radians);

	degrees = bearing + 145.0F;
	radians = DEG2RAD(degrees);
	int p2x = (WIDTH / 2)  + needleRadius * ::cos(radians);
	int p2y = (HEIGHT / 2) + needleRadius * ::sin(radians);

	degrees = bearing + 180.0F;
	radians = DEG2RAD(degrees);
	int p3x = (WIDTH / 2)  + (needleRadius / 2) * ::cos(radians);
	int p3y = (HEIGHT / 2) + (needleRadius / 2) * ::sin(radians);

	degrees = bearing - 145.0F;
	radians = DEG2RAD(degrees);
	int p4x = (WIDTH / 2)  + needleRadius * ::cos(radians);
	int p4y = (HEIGHT / 2) + needleRadius * ::sin(radians);

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

	dc.SetBrush(*wxBLACK_BRUSH);
	dc.SetPen(*wxWHITE_PEN);
	dc.DrawCircle(WIDTH / 2, HEIGHT / 2, COMPASS_RADIUS);

	int x = WIDTH / 2 - 10;
	int y = 5;

	dc.SetPen(*wxBLACK_PEN);
	dc.DrawRectangle(x, y, 20, 20);

	dc.SetPen(*wxWHITE_PEN);

	wxFont font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
	font.SetPointSize(11);
	font.SetWeight(wxFONTWEIGHT_BOLD);

	dc.SetFont(font);

	dc.SetTextForeground(*wxWHITE);

	// North
	x = WIDTH / 2 - 5;
	y = 6;
	dc.DrawText(wxT("N"), x, y);

	dc.SelectObject(wxNullBitmap);
}

