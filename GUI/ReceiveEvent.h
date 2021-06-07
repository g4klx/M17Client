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

#ifndef	ReceiveEvent_H
#define	ReceiveEvent_H

#include "ReceiveData.h"

#include <wx/wx.h>

class CReceiveEvent : public wxEvent {
public:
	CReceiveEvent(CReceiveData* data, wxEventType type, int id = 0);
	virtual ~CReceiveEvent();

	virtual CReceiveData* getData() const;

	virtual wxEvent* Clone() const;

protected:
	CReceiveEvent(const CReceiveEvent& event);

private:
	CReceiveData* m_data;
};

#endif
