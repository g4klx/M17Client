/*
 *   Copyright (C) 2010,2021 by Jonathan Naylor G4KLX
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

#include "TransmitEvent.h"

CTransmitEvent::CTransmitEvent(bool tx, wxEventType type, int id) :
wxEvent(id, type),
m_tx(tx)
{
}

CTransmitEvent::CTransmitEvent(const CTransmitEvent& event) :
wxEvent(event),
m_tx(event.m_tx)
{
}

CTransmitEvent::~CTransmitEvent()
{
}

bool CTransmitEvent::getTX() const
{
	return m_tx;
}

wxEvent* CTransmitEvent::Clone() const
{
	return new CTransmitEvent(*this);
}
