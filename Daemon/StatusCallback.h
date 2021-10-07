/*
 *	Copyright (C) 2009,2015,2021 by Jonathan Naylor, G4KLX
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; version 2 of the License.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 */

#ifndef	StatusCallback_H
#define	StatusCallback_H

#include <string>
#include <optional>

class IStatusCallback {
public:
	virtual void statusCallback(const std::string& source, const std::string& dest, bool start) = 0;

	virtual void textCallback(const char* text) = 0;

	virtual void rssiCallback(int rssi) = 0;

	virtual void gpsCallback(float latitude, float longitude, const std::string& locator,
			const std::optional<float>& altitude,
			const std::optional<float>& speed, const std::optional<float>& track,
			const std::optional<float>& bearing, const std::optional<float>& distance) = 0;

	virtual void callsignsCallback(const char* callsigns) = 0;

private:
};

#endif
