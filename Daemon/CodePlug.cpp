/*
 *   Copyright (C) 2015-2021 by Jonathan Naylor G4KLX
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

#include "CodePlug.h"
#include "Log.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

const int BUFFER_SIZE = 500;

CCodePlug::CCodePlug(const std::string& file) :
m_file(file),
m_data()
{
}

CCodePlug::~CCodePlug()
{
}

bool CCodePlug::read()
{
	FILE* fp = ::fopen(m_file.c_str(), "rt");
	if (fp == NULL) {
		::fprintf(stderr, "Couldn't open the code plug file - %s\n", m_file.c_str());
		return false;
	}

	std::string  name;
	unsigned int txFrequency = 0U;
	unsigned int rxFrequency = 0U;
	unsigned int can  = 99U;
	unsigned int mode = 99U;

	char buffer[BUFFER_SIZE];
	while (::fgets(buffer, BUFFER_SIZE, fp) != NULL) {
		if (buffer[0U] == '#')
			continue;

		if (buffer[0U] == '[') {
			name.clear();
			txFrequency = 0U;
			rxFrequency = 0U;
			can  = 99U;
			mode = 99U;

			char* p;
			p = ::strchr(buffer, ']');
			if (p == NULL)
				continue;
			*p = '\0';

			name = std::string(buffer + 1U);

			continue;
		}

		char* key   = ::strtok(buffer, " \t=\r\n");
		if (key == NULL)
			continue;

		char* value = ::strtok(NULL, "\r\n");
		if (value == NULL)
			continue;

		// Remove quotes from the value
		size_t len = ::strlen(value);
		if (len > 1U && *value == '"' && value[len - 1U] == '"') {
			value[len - 1U] = '\0';
			value++;
		} else {
			// if value is not quoted, remove after # (to make comment)
			(void)::strtok(value, "#");
		}

		if (::strcmp(key, "Frequency") == 0)
			txFrequency = rxFrequency = (unsigned int)::atoi(value);
		else if (::strcmp(key, "TXFrequency") == 0)
			txFrequency = (unsigned int)::atoi(value);
		else if (::strcmp(key, "RXFrequency") == 0)
			rxFrequency = (unsigned int)::atoi(value);
		else if (::strcmp(key, "CAN") == 0)
			can = (unsigned int)::atoi(value);
		else if (::strcmp(key, "Mode") == 0)
			mode = (unsigned int)::atoi(value);

		if (!name.empty() && txFrequency > 0U && rxFrequency > 0U && can != 99U && mode != 99U) {
			CCodePlugData data(name, txFrequency, rxFrequency, can, mode);
			m_data.push_back(data);

			name.clear();
			txFrequency = 0U;
			rxFrequency = 0U;
			can  = 99U;
			mode = 99U;
		}
	}

	::fclose(fp);

	return !m_data.empty();
}

std::vector<CCodePlugData> CCodePlug::getData() const
{
	return m_data;
}

