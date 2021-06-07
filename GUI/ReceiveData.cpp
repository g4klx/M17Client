/*
 *   Copyright (C) 2011,2021 by Jonathan Naylor G4KLX
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

#include "ReceiveData.h"

CReceiveData::CReceiveData(const wxString& source, const wxString& destination, bool end) :
m_source(source),
m_destination(destination),
m_end(end)
{
}

CReceiveData::~CReceiveData()
{
}

wxString CReceiveData::getSource() const
{
	return m_source;
}

wxString CReceiveData::getDestination() const
{
	return m_destination;
}

bool CReceiveData::getEnd() const
{
	return m_end;
}

