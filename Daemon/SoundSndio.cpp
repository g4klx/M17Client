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

#include "SoundSndio.h"
#include "Log.h"

#include <cassert>

#define PlaybackStr "playback"
#define CaptureStr "capture"
#define isCapture(x) ((x) == CaptureStr)

#define byte2sample(x) ((x) / sizeof(*m_temp))
#define sample2byte(x) ((x) * sizeof(*m_temp))

CSoundSndio::CSoundSndio(const std::string& readDevice, const std::string& writeDevice, unsigned int sampleRate, unsigned int blockSize) :
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

CSoundSndio::~CSoundSndio()
{
}

void CSoundSndio::setCallback(IAudioCallback* callback, int id)
{
	assert(callback != NULL);

	m_callback = callback;

	m_id = id;
}

struct sio_hdl* CSoundSndio::setup(std::string device, std::string mode, unsigned int channels)
{
	struct sio_hdl* handle;
	if ((handle = ::sio_open(device.c_str(), isCapture(mode) ? SIO_REC : SIO_PLAY, 0)) == NULL) {
		LogError("Cannot open %s audio device %s", mode.c_str(), device.c_str());
		goto fail;
	}

	struct sio_par q, r;
	sio_initpar(&q);
	q.bits = 16;
	q.bps = SIO_BPS(q.bits);
	q.sig = 1;
	q.le = SIO_LE_NATIVE;
	q.msb = 0;
	q.rchan = q.pchan = channels;
	q.rate = m_sampleRate;
	q.xrun = SIO_IGNORE;
	q.appbufsz = m_blockSize * q.bps;

	if (!sio_setpar(handle, &q)) {
		LogError("Cannot set parameter to %s audio device (%s)", mode.c_str(), device.c_str());
		goto fail;
	}
	if (!sio_getpar(handle, &r)) {
		LogError("Cannot get parameter from %s audio device (%s)", mode.c_str(), device. c_str());
		goto fail;
	}

	// check parameter is set correctly
	if (q.bits != r.bits || q.bps != r.bps || q.sig != r.sig ||
	    q.le != r.le || q.msb != r.msb ||
	    (isCapture(mode) && q.rchan != r.rchan) ||
	    (!isCapture(mode) && q.pchan != r.pchan) ||
	    q.rate != r.rate || q.xrun != r.xrun || q.appbufsz != r.appbufsz) {
		LogError("Failed to set parameter to %s audio device (%s)", mode.c_str(), device.c_str());
		goto fail;
	}

	return handle;

fail:
	if (handle != NULL) ::sio_close(handle);
	return NULL;
}

bool CSoundSndio::open()
{
#define playChannels 1U
#define recChannels 1U

	struct sio_hdl* playHandle = setup(m_writeDevice, PlaybackStr, playChannels);
	struct sio_hdl* recHandle = setup(m_readDevice, CaptureStr, recChannels);

	if (playHandle == NULL || recHandle == NULL)
		return false;

	LogMessage("Opened %s:%s Rate %u", m_writeDevice.c_str(), m_readDevice.c_str(), m_sampleRate);

	m_reader = new CSoundSndioReader(recHandle, m_blockSize, recChannels, m_callback, m_id);
	m_writer = new CSoundSndioWriter(playHandle, m_blockSize, playChannels, m_callback, m_id);

	m_reader->run();
	m_writer->run();

	return true;
}

void CSoundSndio::close()
{
	m_reader->kill();
	m_writer->kill();

	m_reader->wait();
	m_writer->wait();
}

bool CSoundSndio::isWriterBusy() const
{
	return m_writer->isBusy();
}

CSoundSndioReader::CSoundSndioReader(struct sio_hdl* handle, unsigned int blockSize, unsigned int channels, IAudioCallback* callback, int id) :
CThread(),
m_handle(handle),
m_blockSize(blockSize),
m_channels(channels),
m_callback(callback),
m_id(id),
m_killed(false),
m_samples(NULL),
m_temp(NULL)
{
	assert(handle != NULL);
	assert(blockSize > 0U);
	assert(channels == 1U);
	assert(callback != NULL);

	m_samples = new float[blockSize];
	m_temp = new short[blockSize];
}

CSoundSndioReader::~CSoundSndioReader()
{
	delete[] m_samples;
	delete[] m_temp;
}

void CSoundSndioReader::entry()
{
	LogMessage("Starting sndio reader thread");
	if (!::sio_start(m_handle)) {
		LogError("Cannot start playback audio device");
		kill();
	}

	while (!m_killed) {
		unsigned int i, n, pos, remain;
		for (pos = 0; pos < m_blockSize; pos += n) {
			remain = m_blockSize - pos;
			n = ::sio_read(m_handle, m_temp, sample2byte(remain));
			if ((n = byte2sample(n)) > 0) {
				for (i = 0; i < n; i++) m_samples[i + pos] = m_temp[i] / 32768.0f;
			} else {
				sleep(5UL);
			}
		}
		m_callback->readCallback(m_samples, m_blockSize, m_id);
	}

	LogMessage("Stopping sndio reader thread");

	::sio_stop(m_handle);
	::sio_close(m_handle);
}

void CSoundSndioReader::kill()
{
	m_killed = true;
}

CSoundSndioWriter::CSoundSndioWriter(struct sio_hdl* handle, unsigned int blockSize, unsigned int channels, IAudioCallback* callback, int id) :
CThread(),
m_handle(handle),
m_blockSize(blockSize),
m_channels(channels),
m_callback(callback),
m_id(id),
m_killed(false),
m_busy(false),
m_samples(NULL),
m_temp(NULL)
{
	assert(handle != NULL);
	assert(blockSize > 0U);
	assert(channels == 1U);
	assert(callback != NULL);

	m_samples = new float[2U * blockSize];
	m_temp = new short[2U * blockSize];
}

CSoundSndioWriter::~CSoundSndioWriter()
{
	delete[] m_samples;
	delete[] m_temp;
}

void CSoundSndioWriter::entry()
{
	LogMessage("Starting sndio writer thread");
	if (!::sio_start(m_handle)) {
		LogError("Cannot start capture audio device");
		kill();
	}

	while (!m_killed) {
		int nSamples = 2 * m_blockSize;
		m_callback->writeCallback(m_samples, nSamples, m_id);

		if (nSamples == 0) {
			sleep(5UL);
		} else {
			for (int i = 0; i < nSamples; i++) m_temp[i] = m_samples[i] * 32768.0f;
			m_busy = true;
			::sio_write(m_handle, m_temp, sample2byte(nSamples));
			m_busy = false;
		}
	}

	LogMessage("Stopping sndio writer thread");

	::sio_stop(m_handle);
	::sio_close(m_handle);
}

void CSoundSndioWriter::kill()
{
	m_killed = true;
}

bool CSoundSndioWriter::isBusy() const
{
	return m_busy;
}

