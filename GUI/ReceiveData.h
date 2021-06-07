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

#ifndef	ReceiveData_H
#define	ReceiveData_H

#include <wx/wx.h>

class CReceiveData {
public:
	CReceiveData(const wxString& source, const wxString& destination, bool end);
	~CReceiveData();

	wxString getSource() const;
	wxString getDestination() const;
	bool     getEnd() const;

private:
	wxString m_source;
	wxString m_destination;
	bool     m_end;
};

#endif
