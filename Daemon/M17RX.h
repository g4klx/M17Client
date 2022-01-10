/*
 *   Copyright (C) 2020,2021,2022 by Jonathan Naylor G4KLX
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

#include <samplerate.h>

#include <string>
#include <optional>

class CM17RX {
public:
	CM17RX(const std::string& callsign, CRSSIInterpolator* rssiMapper, bool bleep, CCodec2& codec3200, CCodec2& codec1600);
	~CM17RX();

	void setStatusCallback(IStatusCallback* callback);

	unsigned int getVolume() const;

	void setVolume(unsigned int percentage);

	void setGPS(float latitude, float longitude);

	bool write(unsigned char* data, unsigned int len);

	unsigned int read(float* audio, unsigned int len);

private:
	CCodec2&             m_3200;
	CCodec2&             m_1600;
	std::string          m_callsign;
	bool                 m_bleep;
	float                m_volume;
	IStatusCallback*     m_callback;
	RPT_RF_STATE         m_state;
	unsigned int         m_frames;
	unsigned int         m_errs;
	unsigned int         m_bits;
	CM17LSF              m_lsf;
	CM17LSF              m_running;
	uint8_t              m_textBitMap;
	char*                m_text;
	std::string          m_callsigns;
	CRingBuffer<float>   m_queue;
	CRSSIInterpolator*   m_rssiMapper;
	unsigned char        m_rssi;
	unsigned char        m_maxRSSI;
	unsigned char        m_minRSSI;
	unsigned int         m_aveRSSI;
	unsigned int         m_rssiCount;
	SRC_STATE*           m_resampler;
	int                  m_error;
	std::optional<float> m_latitude;
	std::optional<float> m_longitude;

	void writeQueue(const float *audio, unsigned int len);

	bool processHeader(bool lateEntry);

	void interleaver(const unsigned char* in, unsigned char* out) const;
	void decorrelator(const unsigned char* in, unsigned char* out) const;

	void processRunningLSF(const unsigned char* fragment);
	void processLSF(const CM17LSF& lsf);

	void addSilence(unsigned int n);

	void calcBD(const std::optional<float>& srcLat, const std::optional<float>& srcLon,
			float dstLat, float dstLon, std::optional<float>& bearing, std::optional<float>& distance) const;
	std::string calcLocator(float latitude, float longitude) const;

	void end();

	void addBleep();
	void addEnd();
};

#endif
