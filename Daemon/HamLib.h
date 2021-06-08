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

#if defined(USE_HAMLIB)

#if !defined(HamLib_H)
#define	HanLib_H

#include <hamlib/rig.h>

#include <string>

class CHamLib {
public:
	CHamLib(const std::string& type);
	~CHamLib();

	bool open();

	void setFrequency(unsigned int rx, unsigned int tx);

	void close();

private:
	std::string m_type;
	RIG*        m_rig;
};

#endif

#endif

