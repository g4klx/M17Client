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

#if !defined(M17TX_H)
#define	M17TX_H

#include "codec2/codec2.h"
#include "M17Defines.h"
#include "RingBuffer.h"
#include "Defines.h"
#include "M17LSF.h"
#include "Modem.h"

#include <samplerate.h>

#include <string>
#include <vector>
#include <optional>

enum TX_STATUS {
	TXS_NONE,
	TXS_HEADER,
	TXS_AUDIO,
	TXS_END
};

class CM17TX {
public:
	CM17TX(const std::string& callsign, const std::string& text, unsigned int micGain, CCodec2& codec3200, CCodec2& codec1600);
	~CM17TX();

	void setParams(unsigned int can, unsigned int mode);

	void setDestination(const std::string& callsign);

	void setGPS(float latitude, float longitude,
			std::optional<float>& altitude,
			std::optional<float>& speed, std::optional<float>& track,
			const std::string& type);

	void start();

	void write(const float* audio, unsigned int len);

	void process();

	void end();

	unsigned int read(unsigned char* data);

	bool isTX() const;

private:
	CCodec2&                   m_3200;
	CCodec2&                   m_1600;
	unsigned int               m_mode;
	std::string                m_source;
	std::string                m_dest;
	float                      m_micGain;
	unsigned int               m_can;
	TX_STATUS                  m_status;
	CRingBuffer<float>         m_audio;
	CRingBuffer<unsigned char> m_queue;
	uint16_t                   m_frames;
	CM17LSF*                   m_currLSF;
	std::vector<CM17LSF*>::const_iterator m_currTextLSF;
	std::vector<CM17LSF*>      m_textLSF;
	bool                       m_sendingGPS;
	CM17LSF*                   m_gpsLSF;
	unsigned int               m_lsfN;
	SRC_STATE*                 m_resampler;
	int                        m_error;

	void writeQueue(const unsigned char* data);

	void interleaver(const unsigned char* in, unsigned char* out) const;
	void decorrelator(const unsigned char* in, unsigned char* out) const;

	void addLinkSetupSync(unsigned char* data);
	void addStreamSync(unsigned char* data);
	void addEOTSync(unsigned char* data);
};

#endif
