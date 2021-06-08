/*
 *	Copyright (C) 2009,2010,2015,2021 by Jonathan Naylor, G4KLX
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; version 2 of the License.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 */

#ifndef	SoundCard_H
#define	SoundCard_H

#include "AudioCallback.h"

#include <vector>
#include <string>

#include <portaudio.h>

class CSoundCard {
public:
	CSoundCard(const std::string& readDevice, const std::string& writeDevice, unsigned int sampleRate, unsigned int blockSize);
	~CSoundCard();

	void setCallback(IAudioCallback* callback, int id = 0);

	bool open();
	void close();

	void callback(const float* input, float* output, unsigned int nSamples);

	static std::vector<std::string> getReadDevices();
	static std::vector<std::string> getWriteDevices();

private:
	std::string     m_readDevice;
	std::string     m_writeDevice;
	unsigned int    m_sampleRate;
	unsigned int    m_blockSize;
	IAudioCallback* m_callback;
	int             m_id;
	PaStream*       m_stream;

	static std::string m_openReadDevice;
	static std::string m_openWriteDevice;

	bool convertNameToDevices(PaDeviceIndex& inDev, PaDeviceIndex& outDev);
};

#endif
