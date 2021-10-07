/*
 *   Copyright (C) 2010,2021 by Jonathan Naylor G4KLX
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

#ifndef	GPSEvent_H
#define	GPSEvent_H

#include <wx/wx.h>

#include <optional>

class CGPSEvent : public wxEvent {
public:
	CGPSEvent(float latitude, float longitude, const wxString& locator,
			const std::optional<float>& altitude,
			const std::optional<float>& speed, const std::optional<float>& track,
			const std::optional<float>& bearing, const std::optional<float>& distance,
			wxEventType type, int id = 0);
	virtual ~CGPSEvent();

	virtual float                getLatitude() const;
	virtual float                getLongitude() const;
	virtual wxString             getLocator() const;
	virtual std::optional<float> getAltitude() const;
	virtual std::optional<float> getSpeed() const;
	virtual std::optional<float> getTrack() const;
	virtual std::optional<float> getBearing() const;
	virtual std::optional<float> getDistance() const;

	virtual wxEvent* Clone() const;

protected:
	CGPSEvent(const CGPSEvent& event);

private:
	float                m_latitude;
	float                m_longitude;
	wxString             m_locator;
	std::optional<float> m_altitude;
	std::optional<float> m_speed;
	std::optional<float> m_track;
	std::optional<float> m_bearing;
	std::optional<float> m_distance;
};

#endif
