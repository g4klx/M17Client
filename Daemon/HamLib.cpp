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

#include "HamLib.h"

#if defined(USE_HAMLIB)

#include "Log.h"

#include <vector>

#include <cassert>
#include <cstdio>

struct RigDesc {
	std::string m_name;
	rig_model_t m_number;
};

const std::vector<RigDesc> RigNames = {
	{"FT-817",  RIG_MODEL_FT817},
#if defined(RIG_MODEL_FT818)
	{"FT-818",  RIG_MODEL_FT818},
#endif
#if defined(RIG_MODEL_IC705)
	{"IC-705",  RIG_MODEL_IC705},
#endif
	{"IC-7000", RIG_MODEL_IC7000},
	{"IC-7100", RIG_MODEL_IC7100},
	{"TM-D700", RIG_MODEL_TMD700},
	{"TM-D710", RIG_MODEL_TMD710},
	{"TM-V71",  RIG_MODEL_TMV71},
	{"TM-V7",   RIG_MODEL_TMV7}};

CHamLib::CHamLib(const std::string& type) :
m_type(type)
{
	assert(!type.empty());
}

CHamLib::~CHamLib()
{
}

bool CHamLib::open()
{
	rig_model_t number;
	bool found = false;
	for (const auto& it : RigNames) {
		if (it.m_name == m_type) {
			number = it.m_number;
			found  = true;
			break;
		}		
	}

	if (!found) {
		LogError("Unknown rig name - %s", m_type.c_str());
		return false;
	}

	m_rig = ::rig_init(number);
	if (m_rig == NULL) {
		LogError("Invalid rig initialisation");
		return false;
	}

	int ret = ::rig_open(m_rig);
	if (ret != RIG_OK) {
		LogError("Error when opening HamLib, returned %d", ret);
		return false;
	}

	LogMessage("Opened the HamLib interface");

	return true;
}

void CHamLib::setFrequency(unsigned int rx, unsigned int tx)
{
	assert(m_rig != NULL);

	if (rx == tx) {
		int ret = ::rig_set_freq(m_rig, RIG_VFO_CURR, double(rx));
		if (ret != RIG_OK)
			LogError("Error when setting the frequency, returned %d", ret);
	} else {
		int ret = ::rig_set_freq(m_rig, RIG_VFO_RX, double(rx));
		if (ret != RIG_OK)
			LogError("Error when setting the RX frequency, returned %d", ret);

		ret = ::rig_set_freq(m_rig, RIG_VFO_TX, double(tx));
		if (ret != RIG_OK)
			LogError("Error when setting the TX frequency, returned %d", ret);
	}
}

void CHamLib::close()
{
	assert(m_rig != NULL);

	::rig_close(m_rig);
	m_rig = NULL;
}

#endif

