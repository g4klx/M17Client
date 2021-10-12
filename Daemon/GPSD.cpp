/*
 *   Copyright (C) 2018,2020,2021 by Jonathan Naylor G4KLX
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

#include "GPSD.h"
#include "Defines.h"
#include "Log.h"

#if defined(USE_GPSD)

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cmath>

CGPSD::CGPSD(const std::string& address, const std::string& port) :
m_gpsdAddress(address),
m_gpsdPort(port),
m_gpsdData(),
m_timer(1000U, 30U)
{
	assert(!address.empty());
	assert(!port.empty());
}

CGPSD::~CGPSD()
{
}

bool CGPSD::open()
{
	int ret = ::gps_open(m_gpsdAddress.c_str(), m_gpsdPort.c_str(), &m_gpsdData);
	if (ret != 0) {
		LogError("Error when opening access to gpsd - %d - %s", errno, ::gps_errstr(errno));
		return false;
	}

	::gps_stream(&m_gpsdData, WATCH_ENABLE | WATCH_JSON, NULL);

	LogMessage("Connected to GPSD");

	m_timer.start();

	return true;
}

void CGPSD::close()
{
	::gps_stream(&m_gpsdData, WATCH_DISABLE, NULL);
	::gps_close(&m_gpsdData);
}

void CGPSD::clock(unsigned int ms)
{
	m_timer.clock(ms);
}

bool CGPSD::getData(float& latitude, float& longitude, std::optional<float>& altitude, std::optional<float>& speed, std::optional<float>& track)
{
	if (!::gps_waiting(&m_gpsdData, 0))
		return false;

#if GPSD_API_MAJOR_VERSION >= 7
	if (::gps_read(&m_gpsdData, NULL, 0) <= 0)
		return false;
#else
	if (::gps_read(&m_gpsdData) <= 0)
		return false;
#endif

	if (m_timer.isRunning() && !m_timer.hasExpired())
		return false;

#if GPSD_API_MAJOR_VERSION >= 10
	if (m_gpsdData.fix.mode == MODE_NOT_SEEN || m_gpsdData.fix.mode == MODE_NO_FIX)
		return false;
#else
	if (m_gpsdData.status != STATUS_FIX)
		return false;
#endif

	bool latlonSet = (m_gpsdData.set & LATLON_SET) == LATLON_SET;
	if (!latlonSet)
		return false;

	bool altitudeSet = (m_gpsdData.set & ALTITUDE_SET) == ALTITUDE_SET;
	bool speedSet    = (m_gpsdData.set & SPEED_SET) == SPEED_SET;
	bool trackSet    = (m_gpsdData.set & TRACK_SET) == TRACK_SET;

	latitude  = float(m_gpsdData.fix.latitude);
	longitude = float(m_gpsdData.fix.longitude);

	altitude.reset();
	if (altitudeSet) {
#if GPSD_API_MAJOR_VERSION >= 9
		altitude = float(m_gpsdData.fix.altMSL);
		if (isnanf(altitude.value()))
			altitude = float(m_gpsdData.fix.altitude);
#else
		altitude = float(m_gpsdData.fix.altitude);
#endif
	}

	speed.reset();
	track.reset();
	if (speedSet && trackSet) {
		speed = float(m_gpsdData.fix.speed);
		track = float(m_gpsdData.fix.track);
	}

	m_timer.start();

	return true;
}

#endif
