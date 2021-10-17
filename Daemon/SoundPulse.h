/*
 *	Copyright (C) 2009,2010,2015,2021 by Jonathan Naylor, G4KLX
 *	Copyright (C) 2014 by John Wiseman, G8BPQ
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

#ifndef	PulseAudio_H
#define	PulseAudio_H

#include "AudioBackend.h"
#include "AudioCallback.h"
#include "Thread.h"

#include <vector>
#include <string>

#include <pulse/simple.h>

class CSoundPulseReader : public CThread {
public:
	CSoundPulseReader(pa_simple* handle, unsigned int blockSize, unsigned int channels, IAudioCallback* callback, int id);
	virtual ~CSoundPulseReader();

	virtual void entry();

	virtual void kill();

private:
	pa_simple*      m_handle;
	unsigned int    m_blockSize;
	unsigned int    m_channels;
	IAudioCallback* m_callback;
	int             m_id;
	bool            m_killed;
	float*          m_samples;
};

class CSoundPulseWriter : public CThread {
public:
	CSoundPulseWriter(pa_simple* handle, unsigned int blockSize, unsigned int channels, IAudioCallback* callback, int id);
	virtual ~CSoundPulseWriter();

	virtual void entry();

	virtual void kill();

	virtual bool isBusy() const;

private:
	pa_simple*      m_handle;
	unsigned int    m_blockSize;
	unsigned int    m_channels;
	IAudioCallback* m_callback;
	int             m_id;
	bool            m_killed;
	bool volatile   m_busy;
	float*          m_samples;
};

class CSoundPulse : public IAudioBackend {
public:
	CSoundPulse(const std::string& readDevice, const std::string& writeDevice, unsigned int sampleRate, unsigned int blockSize);
	~CSoundPulse();

	void setCallback(IAudioCallback* callback, int id = 0);
	bool open();
	void close();

	bool isWriterBusy() const;

private:
	std::string        m_readDevice;
	std::string        m_writeDevice;
	unsigned int       m_sampleRate;
	unsigned int       m_blockSize;
	IAudioCallback*    m_callback;
	int                m_id;
	CSoundPulseReader* m_reader;
	CSoundPulseWriter* m_writer;
};

#endif
