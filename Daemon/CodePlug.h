/*
 *   Copyright (C) 2015,2016,2017,2019,2020,2021 by Jonathan Naylor G4KLX
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

#if !defined(CODEPLUG_H)
#define	CODEPLUG_H

#include <string>
#include <vector>

class CCodePlugData {
public:
	CCodePlugData(const std::string& name, unsigned int txFrequency, unsigned int rxFrequency, unsigned int can, unsigned int mode) :
	m_name(name),
	m_txFrequency(txFrequency),
	m_rxFrequency(rxFrequency),
	m_can(can),
	m_mode(mode)
	{}

	std::string  m_name;
	unsigned int m_txFrequency;
	unsigned int m_rxFrequency;
	unsigned int m_can;
	unsigned int m_mode;
};

class CCodePlug
{
public:
	CCodePlug(const std::string& file);
	~CCodePlug();

	bool read();

	std::vector<CCodePlugData> getData() const;

private:
	std::string                m_file;
	std::vector<CCodePlugData> m_data;
};

#endif
