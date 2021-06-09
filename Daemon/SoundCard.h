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

#ifndef	SoundCard_H
#define	SoundCard_H

#include "AudioCallback.h"
#include "Thread.h"

#include <vector>
#include <string>

#include <alsa/asoundlib.h>

class CSoundCardReader : public CThread {
public:
	CSoundCardReader(snd_pcm_t* handle, unsigned int blockSize, unsigned int channels, IAudioCallback* callback, int id);
	virtual ~CSoundCardReader();

	virtual void entry();

	virtual void kill();

private:
	snd_pcm_t*      m_handle;
	unsigned int    m_blockSize;
	unsigned int    m_channels;
	IAudioCallback* m_callback;
	int             m_id;
	bool            m_killed;
	float*          m_samples;
};

class CSoundCardWriter : public CThread {
public:
	CSoundCardWriter(snd_pcm_t* handle, unsigned int blockSize, unsigned int channels, IAudioCallback* callback, int id);
	virtual ~CSoundCardWriter();

	virtual void entry();

	virtual void kill();

	virtual bool isBusy() const;

private:
	snd_pcm_t*      m_handle;
	unsigned int    m_blockSize;
	unsigned int    m_channels;
	IAudioCallback* m_callback;
	int             m_id;
	bool            m_killed;
	float*          m_samples;
};

class CSoundCard {
public:
	CSoundCard(const std::string& readDevice, const std::string& writeDevice, unsigned int sampleRate, unsigned int blockSize);
	~CSoundCard();

	void setCallback(IAudioCallback* callback, int id = 0);
	bool open();
	void close();

	bool isWriterBusy() const;

private:
	std::string       m_readDevice;
	std::string       m_writeDevice;
	unsigned int      m_sampleRate;
	unsigned int      m_blockSize;
	IAudioCallback*   m_callback;
	int               m_id;
	CSoundCardReader* m_reader;
	CSoundCardWriter* m_writer;
};

#endif
