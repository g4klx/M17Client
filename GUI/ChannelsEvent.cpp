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

#include "ChannelsEvent.h"

CChannelsEvent::CChannelsEvent(const wxArrayString& channels, wxEventType type, int id) :
wxEvent(id, type),
m_channels(channels)
{
}

CChannelsEvent::CChannelsEvent(const CChannelsEvent& event) :
wxEvent(event),
m_channels(event.m_channels)
{
}

CChannelsEvent::~CChannelsEvent()
{
}

wxArrayString CChannelsEvent::getChannels() const
{
	return m_channels;
}

wxEvent* CChannelsEvent::Clone() const
{
	return new CChannelsEvent(*this);
}
