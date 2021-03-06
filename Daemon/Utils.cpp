/*
 *	Copyright (C) 2009,2014,2015,2016,2021 Jonathan Naylor, G4KLX
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

#include "Utils.h"
#include "Log.h"

#include <cstdio>
#include <cassert>

void CUtils::dump(const std::string& title, const unsigned char* data, unsigned int length)
{
	assert(data != NULL);

	dump(2U, title, data, length);
}

void CUtils::dump(int level, const std::string& title, const unsigned char* data, unsigned int length)
{
	assert(data != NULL);

	::Log(level, "%s", title.c_str());

	unsigned int offset = 0U;

	while (length > 0U) {
		std::string output;

		unsigned int bytes = (length > 16U) ? 16U : length;

		for (unsigned i = 0U; i < bytes; i++) {
			char temp[10U];
			::sprintf(temp, "%02X ", data[offset + i]);
			output += temp;
		}

		for (unsigned int i = bytes; i < 16U; i++)
			output += "   ";

		output += "   *";

		for (unsigned i = 0U; i < bytes; i++) {
			unsigned char c = data[offset + i];

			if (::isprint(c))
				output += c;
			else
				output += '.';
		}

		output += '*';

		::Log(level, "%04X:  %s", offset, output.c_str());

		offset += 16U;

		if (length >= 16U)
			length -= 16U;
		else
			length = 0U;
	}
}

unsigned int CUtils::countBits(unsigned int v)
{
	unsigned int count = 0U;

	while (v != 0U) {
		v &= v - 1U;
		count++;
	}

	return count;
}

