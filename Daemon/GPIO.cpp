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

#include "GPIO.h"

#if defined(USE_GPIO)

#include "Log.h"

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cmath>

#include "cerrno"

CGPIO::CGPIO(bool pttInvert, unsigned int pttPin, bool volumeInvert, unsigned int volumeUpPin, unsigned int volumeDownPin) :
m_pttInvert(pttInvert),
m_pttPin(pttPin),
m_volumeInvert(pttInvert),
m_volumeUpPin(volumeUpPin),
m_volumeDownPin(volumeDownPin),
m_chip(NULL),
m_ptt(NULL),
m_volumeUp(NULL),
m_volumeDown(NULL)
{
}

CGPIO::~CGPIO()
{
}

bool CGPIO::open()
{
	m_chip = ::gpiod_chip_open_by_name("gpiochip0");
	if (m_chip == NULL) {
		LogError("Unable to open the GPIO chip, errno=%d", errno);
		return false;
	}

	if (m_pttPin > 0U) {
		m_ptt = ::gpiod_chip_get_line(m_chip, m_pttPin);
		if (m_ptt == NULL) {
			LogError("Unable to open the PTT GPIO pin, errno=%d", errno);
			return false;
		}

		int ret = ::gpiod_line_request_input(m_ptt, "M17Client");
		if (ret == -1) {
			LogError("Unable to set the PTT GPIO pin for input, errno=%d", errno);
			return false;
		}
	}

	if (m_volumeUpPin > 0U) {
		m_volumeUp = ::gpiod_chip_get_line(m_chip, m_volumeUpPin);
		if (m_volumeUp == NULL) {
			LogError("Unable to open the Volume Up GPIO pin, errno=%d", errno);
			return false;
		}

		int ret = ::gpiod_line_request_input(m_volumeUp, "M17Client");
		if (ret == -1) {
			LogError("Unable to set the Volume Up GPIO pin for input, errno=%d", errno);
			return false;
		}
	}

	if (m_volumeDownPin > 0U) {
		m_volumeDown = ::gpiod_chip_get_line(m_chip, m_volumeDownPin);
		if (m_volumeDown == NULL) {
			LogError("Unable to open the Volume Down GPIO pin, errno=%d", errno);
			return false;
		}

		int ret = ::gpiod_line_request_input(m_volumeDown, "M17Client");
		if (ret == -1) {
			LogError("Unable to set the Volume Down GPIO pin for input, errno=%d", errno);
			return false;
		}
	}

	return true;
}

bool CGPIO::getPTT()
{
	if (m_ptt == NULL)
		return false;
	
	int ret = ::gpiod_line_get_value(m_ptt);
	switch (ret) {
		case 1:
			return m_pttInvert ? true : false;
		case 0:
			return m_pttInvert ? false : true;
		default:
			LogError("Unable to read the PTT GPIO pin, errno=%d", errno);
			return false;
	}
}

bool CGPIO::getVolumeUp()
{
	if (m_volumeUp == NULL)
		return false;
	
	int ret = ::gpiod_line_get_value(m_volumeUp);
	switch (ret) {
		case 1:
			return m_volumeInvert ? true : false;
		case 0:
			return m_volumeInvert ? false : true;
		default:
			LogError("Unable to read the Volume Up GPIO pin, errno=%d", errno);
			return false;
	}
}

bool CGPIO::getVolumeDown()
{
	if (m_volumeDown == NULL)
		return false;
	
	int ret = ::gpiod_line_get_value(m_volumeDown);
	switch (ret) {
		case 1:
			return m_volumeInvert ? true : false;
		case 0:
			return m_volumeInvert ? false : true;
		default:
			LogError("Unable to read the Volume Down GPIO pin, errno=%d", errno);
			return false;
	}
}

void CGPIO::close()
{
	assert(m_chip != NULL);

	if (m_ptt != NULL) {
		::gpiod_line_release(m_ptt);
		m_ptt  = NULL;
	}

	if (m_volumeUp != NULL) {
		::gpiod_line_release(m_volumeUp);
		m_volumeUp  = NULL;
	}

	if (m_volumeDown != NULL) {
		::gpiod_line_release(m_volumeDown);
		m_volumeDown  = NULL;
	}

	::gpiod_chip_close(m_chip);
	m_chip = NULL;
}

#endif
