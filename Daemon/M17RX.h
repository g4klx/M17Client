/*
 *   Copyright (C) 2020,2021 by Jonathan Naylor G4KLX
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

#if !defined(M17RX_H)
#define	M17RX_H

#include "RSSIInterpolator.h"
#include "M17Defines.h"
#include "StopWatch.h"
#include "Defines.h"
#include "M17LSF.h"
#include "Modem.h"

#include <string>

class CM17RX {
public:
	CM17RX(const std::string& callsign, CRSSIInterpolator* rssiMapper);
	~CM17RX();

	void setCAN(unsigned int can);

	bool writeModem(unsigned char* data, unsigned int len);

private:
	std::string                m_callsign;
	unsigned int               m_can;
	RPT_RF_STATE               m_rfState;
	CStopWatch                 m_elapsed;
	unsigned int               m_rfFrames;
	unsigned int               m_rfErrs;
	unsigned int               m_rfBits;
	CM17LSF                    m_rfLSF;
	unsigned int               m_rfLSFn;
	CRSSIInterpolator*         m_rssiMapper;
	unsigned char              m_rssi;
	unsigned char              m_maxRSSI;
	unsigned char              m_minRSSI;
	unsigned int               m_aveRSSI;
	unsigned int               m_rssiCount;

	bool processRFHeader(bool lateEntry);

	void interleaver(const unsigned char* in, unsigned char* out) const;
	void decorrelator(const unsigned char* in, unsigned char* out) const;

	void writeEndRF();
};

#endif
