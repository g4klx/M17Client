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

CGPIO::CGPIO(unsigned int pttPin) :
m_pttPin(pttPin),
m_chip(NULL),
m_ptt(NULL)
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

	m_ptt = ::gpiod_chip_get_line(m_chip, m_pttPin);
	if (m_chip == NULL) {
		LogError("Unable to open the PTT GPIO pin, errno=%d", errno);
		return false;
	}

	int ret = ::gpiod_line_request_input(m_ptt, "M17Client");
	if (ret == -1) {
		LogError("Unable to set the PTT GPIO pin for input, errno=%d", errno);
		return false;
	}

	return true;
}

bool CGPIO::readPTT()
{
	assert(m_ptt != NULL);
	
	int ret = ::gpiod_line_get_value(m_ptt);
	switch (ret) {
		case 1:
			return true;
		case 0:
			return false;
		default:
			LogError("Unable to read the PTT GPIO pin, errno=%d", errno);
			return false;
	}
}

void CGPIO::close()
{
	assert(m_chip != NULL);
	assert(m_ptt != NULL);

	::gpiod_line_release(m_ptt);
	::gpiod_chip_close(m_chip);

	m_chip = NULL;
	m_ptt  = NULL;
}

#endif
