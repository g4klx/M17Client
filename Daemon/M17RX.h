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
#include "StatusCallback.h"
#include "codec2/codec2.h"
#include "RingBuffer.h"
#include "M17Defines.h"
#include "Defines.h"
#include "M17LSF.h"
#include "Modem.h"

#include <string>

class CM17RX {
public:
	CM17RX(const std::string& callsign, CRSSIInterpolator* rssiMapper, bool bleep, CCodec2& codec2);
	~CM17RX();

	void setStatusCallback(IStatusCallback* callback);

	void setCAN(unsigned int can);

	void setVolume(unsigned int percentage);

	bool write(unsigned char* data, unsigned int len);

	unsigned int read(float* audio, unsigned int len);

private:
	CCodec2&           m_codec2;
	std::string        m_callsign;
	bool               m_bleep;
	float              m_volume;
	IStatusCallback*   m_callback;
	unsigned int       m_can;
	RPT_RF_STATE       m_state;
	unsigned int       m_frames;
	CM17LSF            m_lsf;
	CM17LSF            m_running;
	CRingBuffer<float> m_queue;
	CRSSIInterpolator* m_rssiMapper;
	unsigned char      m_rssi;
	unsigned char      m_maxRSSI;
	unsigned char      m_minRSSI;
	unsigned int       m_aveRSSI;
	unsigned int       m_rssiCount;

	void writeQueue(const float *audio, unsigned int len);

	bool processHeader(bool lateEntry);

	void interleaver(const unsigned char* in, unsigned char* out) const;
	void decorrelator(const unsigned char* in, unsigned char* out) const;

	void processRunningLSF(const unsigned char* fragment);
	void processLSF(const CM17LSF& lsf) const;

	void end();

	void addBleep();
};

#endif
