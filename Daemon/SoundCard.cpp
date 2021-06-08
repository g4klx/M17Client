/*
 *   Copyright (C) 2006-2010,2015,2021 by Jonathan Naylor G4KLX
 *   Copyright (C) 2014 by John Wiseman, G8BPQ
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

#include "SoundCard.h"
#include "Log.h"

#include <cassert>

CSoundCard::CSoundCard(const std::string& readDevice, const std::string& writeDevice, unsigned int sampleRate, unsigned int blockSize) :
m_readDevice(readDevice),
m_writeDevice(writeDevice),
m_sampleRate(sampleRate),
m_blockSize(blockSize),
m_callback(NULL),
m_id(-1),
m_reader(NULL),
m_writer(NULL)
{
    assert(sampleRate > 0U);
    assert(blockSize > 0U);
}

CSoundCard::~CSoundCard()
{
}

void CSoundCard::setCallback(IAudioCallback* callback, int id)
{
	assert(callback != NULL);

	m_callback = callback;

	m_id = id;
}

bool CSoundCard::open()
{
	int err = 0;

	snd_pcm_t* playHandle = NULL;
	if ((err = ::snd_pcm_open(&playHandle, m_writeDevice.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		LogError("Cannot open playback audio device %s (%s)", m_writeDevice.c_str(), ::snd_strerror(err));
		return false;
	}

	snd_pcm_hw_params_t* hw_params;
	if ((err = ::snd_pcm_hw_params_malloc(&hw_params)) < 0) {
		LogError("Cannot allocate hardware parameter structure (%s)", ::snd_strerror(err));
		return false;
	}

	if ((err = ::snd_pcm_hw_params_any(playHandle, hw_params)) < 0) {
		LogError("Cannot initialize hardware parameter structure (%s)", ::snd_strerror(err));
		return false;
	}

	if ((err = ::snd_pcm_hw_params_set_access(playHandle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		LogError("Cannot set access type (%s)", ::snd_strerror(err));
		return false;
	}

	if ((err = ::snd_pcm_hw_params_set_format(playHandle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		LogError("Cannot set sample format (%s)", ::snd_strerror(err));
		return false;
	}
	
	if ((err = ::snd_pcm_hw_params_set_rate(playHandle, hw_params, m_sampleRate, 0)) < 0) {
		LogError("Cannot set sample rate (%s)", ::snd_strerror(err));
		return false;
	}

	unsigned int playChannels = 1U;

	if ((err = ::snd_pcm_hw_params_set_channels(playHandle, hw_params, 1)) < 0) {
		playChannels = 2U;

		if ((err = ::snd_pcm_hw_params_set_channels(playHandle, hw_params, 2)) < 0) {
			LogError("Cannot play set channel count (%s)", ::snd_strerror(err));
			return false;
		}
	}
	
	if ((err = ::snd_pcm_hw_params(playHandle, hw_params)) < 0) {
		LogError("Cannot set parameters (%s)", ::snd_strerror(err));
		return false;
	}
	
	::snd_pcm_hw_params_free(hw_params);
	
	if ((err = ::snd_pcm_prepare(playHandle)) < 0) {
		LogError("Cannot prepare audio interface for use (%s)", ::snd_strerror(err));
		return false;
	}

	// Open Capture
	snd_pcm_t* recHandle = NULL;
	if ((err = ::snd_pcm_open(&recHandle, m_readDevice.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		LogError("Cannot open capture audio device %s (%s)", m_readDevice.c_str(), ::snd_strerror(err));
		return false;
	}

	if ((err = ::snd_pcm_hw_params_malloc(&hw_params)) < 0) {
		LogError("Cannot allocate hardware parameter structure (%s)", ::snd_strerror(err));
		return false;
	}

	if ((err = ::snd_pcm_hw_params_any(recHandle, hw_params)) < 0) {
		LogError("Cannot initialize hardware parameter structure (%s)", ::snd_strerror(err));
		return false;
	}
	
	if ((err = ::snd_pcm_hw_params_set_access(recHandle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		LogError("Cannot set access type (%s)", ::snd_strerror(err));
		return false;
	}

	if ((err = ::snd_pcm_hw_params_set_format(recHandle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		LogError("Cannot set sample format (%s)", ::snd_strerror(err));
		return false;
	}
	
	if ((err = ::snd_pcm_hw_params_set_rate(recHandle, hw_params, m_sampleRate, 0)) < 0) {
		LogError("Cannot set sample rate (%s)", ::snd_strerror(err));
		return false;
	}
	
	unsigned int recChannels = 1U;
	
	if ((err = ::snd_pcm_hw_params_set_channels(recHandle, hw_params, 1)) < 0) {
		recChannels = 2U;

		if ((err = ::snd_pcm_hw_params_set_channels (recHandle, hw_params, 2)) < 0) {
			LogError("Cannot rec set channel count (%s)", ::snd_strerror(err));
			return false;
		}
	}
	
	if ((err = ::snd_pcm_hw_params(recHandle, hw_params)) < 0) {
		LogError("Cannot set parameters (%s)", ::snd_strerror(err));
		return false;
	}

	::snd_pcm_hw_params_free(hw_params);

	if ((err = ::snd_pcm_prepare(recHandle)) < 0) {
		LogError("Cannot prepare audio interface for use (%s)", ::snd_strerror(err));
		return false;
	}

	short samples[256];
	for (unsigned int i = 0U; i < 10U; ++i)
		::snd_pcm_readi(recHandle, samples, 128);

	LogMessage("Opened %s %s Rate %u", m_writeDevice.c_str(), m_readDevice.c_str(), m_sampleRate);

	m_reader = new CSoundCardReader(recHandle,  m_blockSize, recChannels,  m_callback, m_id);
	m_writer = new CSoundCardWriter(playHandle, m_blockSize, playChannels, m_callback, m_id);

	m_reader->run();
	m_writer->run();

 	return true;
}

void CSoundCard::close()
{
	m_reader->kill();
	m_writer->kill();

	m_reader->wait();
	m_writer->wait();
}

bool CSoundCard::isWriterBusy() const
{
	return m_writer->isBusy();
}

CSoundCardReader::CSoundCardReader(snd_pcm_t* handle, unsigned int blockSize, unsigned int channels, IAudioCallback* callback, int id) :
CThread(),
m_handle(handle),
m_blockSize(blockSize),
m_channels(channels),
m_callback(callback),
m_id(id),
m_killed(false),
m_samples(NULL)
{
	assert(handle != NULL);
	assert(blockSize > 0U);
	assert(channels == 1U || channels == 2U);
	assert(callback != NULL);

	m_samples = new short[2U * blockSize];
}

CSoundCardReader::~CSoundCardReader()
{
	delete[] m_samples;
}

void CSoundCardReader::entry()
{
	LogMessage("Starting ALSA reader thread");

	while (!m_killed) {
		snd_pcm_sframes_t ret;
		while ((ret = ::snd_pcm_readi(m_handle, m_samples, m_blockSize)) < 0) {
			if (ret != -EPIPE)
				LogWarning("snd_pcm_readi returned %d (%s)", ret, ::snd_strerror(ret));

			::snd_pcm_recover(m_handle, ret, 1);
		}

		m_callback->readCallback(m_samples, (unsigned int)ret, m_id);
	}

	LogMessage("Stopping ALSA reader thread");

	::snd_pcm_close(m_handle);
}

void CSoundCardReader::kill()
{
	m_killed = true;
}

CSoundCardWriter::CSoundCardWriter(snd_pcm_t* handle, unsigned int blockSize, unsigned int channels, IAudioCallback* callback, int id) :
CThread(),
m_handle(handle),
m_blockSize(blockSize),
m_channels(channels),
m_callback(callback),
m_id(id),
m_killed(false),
m_samples(NULL)
{
	assert(handle != NULL);
	assert(blockSize > 0U);
	assert(channels == 1U || channels == 2U);
	assert(callback != NULL);

	m_samples = new short[4U * blockSize];
}

CSoundCardWriter::~CSoundCardWriter()
{
	delete[] m_samples;
}

void CSoundCardWriter::entry()
{
	LogMessage("Starting ALSA writer thread");

	while (!m_killed) {
		int nSamples = 2U * m_blockSize;
		m_callback->writeCallback(m_samples, nSamples, m_id);

		if (nSamples == 0U) {
			sleep(5UL);
		} else {
			int offset = 0U;
			snd_pcm_sframes_t ret;
			while ((ret = ::snd_pcm_writei(m_handle, m_samples + offset, nSamples - offset)) != (nSamples - offset)) {
				if (ret < 0) {
					if (ret != -EPIPE)
						LogWarning("snd_pcm_writei returned %d (%s)", ret, ::snd_strerror(ret));

					::snd_pcm_recover(m_handle, ret, 1);
				} else {
					offset += ret;
				}
			}
		}
	}

	LogMessage("Stopping ALSA writer thread");

	::snd_pcm_close(m_handle);
}

void CSoundCardWriter::kill()
{
	m_killed = true;
}

bool CSoundCardWriter::isBusy() const
{
	snd_pcm_state_t state = ::snd_pcm_state(m_handle);

	return state == SND_PCM_STATE_RUNNING || state == SND_PCM_STATE_DRAINING;
}

