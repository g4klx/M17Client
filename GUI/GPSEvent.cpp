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

#include "GPSEvent.h"

CGPSEvent::CGPSEvent(float latitude, float longitude, const wxString& locator,
			const std::optional<float>& altitude,
			const std::optional<float>& speed, const std::optional<float>& track,
			const std::optional<float>& bearing, const std::optional<float>& distance,
			wxEventType type, int id) :
wxEvent(id, type),
m_latitude(latitude),
m_longitude(longitude),
m_locator(locator),
m_altitude(altitude),
m_speed(speed),
m_track(track),
m_bearing(bearing),
m_distance(distance)
{
}

CGPSEvent::CGPSEvent(const CGPSEvent& event) :
wxEvent(event),
m_latitude(event.m_latitude),
m_longitude(event.m_longitude),
m_locator(event.m_locator),
m_altitude(event.m_altitude),
m_speed(event.m_speed),
m_track(event.m_track),
m_bearing(event.m_bearing),
m_distance(event.m_distance)
{
}

CGPSEvent::~CGPSEvent()
{
}

float CGPSEvent::getLatitude() const
{
	return m_latitude;
}

float CGPSEvent::getLongitude() const
{
	return m_longitude;
}

wxString CGPSEvent::getLocator() const
{
	return m_locator;
}

std::optional<float> CGPSEvent::getAltitude() const
{
	return m_altitude;
}

std::optional<float> CGPSEvent::getSpeed() const
{
	return m_speed;
}

std::optional<float> CGPSEvent::getTrack() const
{
	return m_track;
}

std::optional<float> CGPSEvent::getBearing() const
{
	return m_bearing;
}

std::optional<float> CGPSEvent::getDistance() const
{
	return m_distance;
}

wxEvent* CGPSEvent::Clone() const
{
	return new CGPSEvent(*this);
}
