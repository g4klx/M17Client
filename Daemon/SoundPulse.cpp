/*
 *   Copyright (C) 2006-2010,2015,2021,2024 by Jonathan Naylor G4KLX
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

#include "SoundPulse.h"
#include "Log.h"

#include <cassert>

#if defined(__linux__)
#include <endian.h>
#elif defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__NetBSD__)
#include <sys/endian.h>
#else
#error Platform not supported
#endif

CSoundPulse::CSoundPulse(const std::string& readDevice, const std::string& writeDevice, unsigned int sampleRate, unsigned int blockSize) :
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

CSoundPulse::~CSoundPulse()
{
}

void CSoundPulse::setCallback(IAudioCallback* callback, int id)
{
	assert(callback != NULL);

	m_callback = callback;

	m_id = id;
}

bool CSoundPulse::open()
{
	pa_sample_spec ss;
#if __BYTE_ORDER == __LITTLE_ENDIAN
	ss.format = PA_SAMPLE_FLOAT32LE;
#else
	ss.format = PA_SAMPLE_FLOAT32BE;
#endif
	ss.rate = m_sampleRate;
	ss.channels = 1;

	pa_simple* playHandle = ::pa_simple_new(NULL, "M17Client", PA_STREAM_PLAYBACK, (m_writeDevice == "default") ? NULL : m_writeDevice.c_str(), "Receive", &ss, NULL, NULL, NULL);
	if (!playHandle) {
		LogError("Cannot open playback audio device %s", m_writeDevice.c_str());
		return false;
	}

	pa_simple* recHandle = ::pa_simple_new(NULL, "M17Client", PA_STREAM_RECORD, (m_readDevice == "default") ? NULL : m_readDevice.c_str(), "Transmit", &ss, NULL, NULL, NULL);
	if (!recHandle) {
		LogError("Cannot open capture audio device %s", m_readDevice.c_str());
		return false;
	}

	LogMessage("Opened %s:%s Rate %u", m_writeDevice.c_str(), m_readDevice.c_str(), m_sampleRate);

	m_reader = new CSoundPulseReader(recHandle,  m_blockSize, ss.channels, m_callback, m_id);
	m_writer = new CSoundPulseWriter(playHandle, m_blockSize, ss.channels, m_callback, m_id);

	m_reader->run();
	m_writer->run();

	return true;
}

void CSoundPulse::close()
{
	m_reader->kill();
	m_writer->kill();

	m_reader->wait();
	m_writer->wait();
}

bool CSoundPulse::isWriterBusy() const
{
	return m_writer->isBusy();
}

CSoundPulseReader::CSoundPulseReader(pa_simple* handle, unsigned int blockSize, unsigned int channels, IAudioCallback* callback, int id) :
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

	m_samples = new float[4U * blockSize];
}

CSoundPulseReader::~CSoundPulseReader()
{
	delete[] m_samples;
}

void CSoundPulseReader::entry()
{
	LogMessage("Starting PulseAudio reader thread");

	while (!m_killed) {
		int err;
		if (::pa_simple_read(m_handle, (uint8_t *)m_samples, m_blockSize * sizeof(float), &err) < 0) {
			LogWarning("pa_simple_read error %d", err);
			sleep(5UL);
		} else {
			m_callback->readCallback(m_samples, m_blockSize, m_id);
		}
	}

	LogMessage("Stopping PulseAudio reader thread");

	::pa_simple_free(m_handle);
}

void CSoundPulseReader::kill()
{
	m_killed = true;
}

CSoundPulseWriter::CSoundPulseWriter(pa_simple* handle, unsigned int blockSize, unsigned int channels, IAudioCallback* callback, int id) :
CThread(),
m_handle(handle),
m_blockSize(blockSize),
m_channels(channels),
m_callback(callback),
m_id(id),
m_killed(false),
m_busy(false),
m_samples(NULL)
{
	assert(handle != NULL);
	assert(blockSize > 0U);
	assert(channels == 1U || channels == 2U);
	assert(callback != NULL);

	m_samples = new float[4U * blockSize];
}

CSoundPulseWriter::~CSoundPulseWriter()
{
	delete[] m_samples;
}

void CSoundPulseWriter::entry()
{
	LogMessage("Starting PulseAudio writer thread");

	while (!m_killed) {
		int nSamples = 2 * m_blockSize;
		m_callback->writeCallback(m_samples, nSamples, m_id);

		if (nSamples == 0) {
			sleep(5UL);
		} else {
			int err;
			m_busy = true;
			if (::pa_simple_write(m_handle, (uint8_t *)m_samples, nSamples * sizeof(float), &err) < 0)  {
				LogWarning("pa_simple_write error %d", err);
			}
			m_busy = false;
		}
	}

	LogMessage("Stopping PulseAudio writer thread");

	::pa_simple_free(m_handle);
}

void CSoundPulseWriter::kill()
{
	m_killed = true;
}

bool CSoundPulseWriter::isBusy() const
{
	return m_busy;
}

