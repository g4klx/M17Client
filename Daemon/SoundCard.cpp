/*
 *   Copyright (C) 2006-2010,2015,2021 by Jonathan Naylor G4KLX
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
#include <cstdio>

std::string CSoundCard::m_openReadDevice;
std::string CSoundCard::m_openWriteDevice;

static int scrwCallback(const void* input, void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData)
{
	assert(userData != NULL);

	CSoundCard* object = reinterpret_cast<CSoundCard*>(userData);

	object->callback(static_cast<const float*>(input), static_cast<float*>(output), nSamples);

	return paContinue;
}

CSoundCard::CSoundCard(const std::string& readDevice, const std::string& writeDevice, unsigned int sampleRate, unsigned int blockSize) :
m_readDevice(readDevice),
m_writeDevice(writeDevice),
m_sampleRate(sampleRate),
m_blockSize(blockSize),
m_callback(NULL),
m_id(-1),
m_stream(NULL)
{
}

CSoundCard::~CSoundCard()
{
}

std::vector<std::string> CSoundCard::getReadDevices()
{
	std::vector<std::string> devices;

	if (!m_openReadDevice.empty())
		devices.push_back(m_openReadDevice);

	PaError error = ::Pa_Initialize();
	if (error != paNoError)
		return devices;

	PaHostApiIndex apiIndex = ::Pa_HostApiTypeIdToHostApiIndex(paALSA);
	if (apiIndex == paHostApiNotFound) {
		::Pa_Terminate();
		return devices;
	}

	PaDeviceIndex n = ::Pa_GetDeviceCount();
	if (n <= 0) {
		::Pa_Terminate();
		return devices;
	}

	for (PaDeviceIndex i = 0; i < n; i++) {
		const PaDeviceInfo* device = ::Pa_GetDeviceInfo(i);

		if (device->hostApi != apiIndex)
			continue;

		if (device->maxInputChannels > 0)
			devices.push_back(device->name);
	}

	::Pa_Terminate();

	return devices;
}

std::vector<std::string> CSoundCard::getWriteDevices()
{
	std::vector<std::string> devices;

	if (!m_openWriteDevice.empty())
		devices.push_back(m_openWriteDevice);

	PaError error = ::Pa_Initialize();
	if (error != paNoError)
		return devices;

	PaHostApiIndex apiIndex = ::Pa_HostApiTypeIdToHostApiIndex(paALSA);
	if (apiIndex == paHostApiNotFound) {
		::Pa_Terminate();
		return devices;
	}

	PaDeviceIndex n = ::Pa_GetDeviceCount();
	if (n <= 0) {
		::Pa_Terminate();
		return devices;
	}

	for (PaDeviceIndex i = 0; i < n; i++) {
		const PaDeviceInfo* device = ::Pa_GetDeviceInfo(i);

		if (device->hostApi != apiIndex)
			continue;

		if (device->maxOutputChannels > 0)
			devices.push_back(device->name);
	}

	::Pa_Terminate();

	return devices;
}

void CSoundCard::setCallback(IAudioCallback* callback, int id)
{
	assert(callback != NULL);

	m_callback = callback;
	m_id       = id;
}

bool CSoundCard::open()
{
	m_openReadDevice.clear();
	m_openWriteDevice.clear();

	PaError error = ::Pa_Initialize();
	if (error != paNoError) {
		LogError("Cannot initialise PortAudio");
		return false;
	}

	PaStreamParameters* pParamsIn  = NULL;
	PaStreamParameters* pParamsOut = NULL;

	PaStreamParameters paramsIn;
	PaStreamParameters paramsOut;

	PaDeviceIndex inDev, outDev;
	bool res = convertNameToDevices(inDev, outDev);
	if (!res) {
		LogError("Cannot convert name to device");
		return false;
	}

	if (inDev != -1) {
		const PaDeviceInfo* inInfo  = ::Pa_GetDeviceInfo(inDev);
		if (inInfo == NULL) {
			LogError("Cannot get device information for the input device");
			return false;
		}

		paramsIn.device                    = inDev;
		paramsIn.channelCount              = 1;
		paramsIn.sampleFormat              = paFloat32;
		paramsIn.hostApiSpecificStreamInfo = NULL;
		paramsIn.suggestedLatency          = inInfo->defaultLowInputLatency;

		pParamsIn = &paramsIn;
	}

	if (outDev != -1) {
		const PaDeviceInfo* outInfo = ::Pa_GetDeviceInfo(outDev);
		if (outInfo == NULL) {
			LogError("Cannot get device information for the output device");
			return false;
		}

		paramsOut.device                    = outDev;
		paramsOut.channelCount              = 1;
		paramsOut.sampleFormat              = paFloat32;
		paramsOut.hostApiSpecificStreamInfo = NULL;
		paramsOut.suggestedLatency          = outInfo->defaultLowOutputLatency;

		pParamsOut = &paramsOut;
	}

	error = ::Pa_OpenStream(&m_stream, pParamsIn, pParamsOut, double(m_sampleRate), m_blockSize, paNoFlag, &scrwCallback, this);
	if (error != paNoError) {
		LogError("Cannot open the audios stream(s)");
		::Pa_Terminate();
		return false;
	}

	error = ::Pa_StartStream(m_stream);
	if (error != paNoError) {
		LogError("Cannot start the audio stream(s)");
		::Pa_CloseStream(m_stream);
		m_stream = NULL;

		::Pa_Terminate();
		return false;
	}

	m_openReadDevice  = m_readDevice;
	m_openWriteDevice = m_writeDevice;

	return true;
}

void CSoundCard::close()
{
	assert(m_stream != NULL);

	::Pa_AbortStream(m_stream);

	::Pa_CloseStream(m_stream);

	::Pa_Terminate();
}

void CSoundCard::callback(const float* input, float* output, unsigned int nSamples)
{
	if (m_callback != NULL) {
		m_callback->readCallback(input, nSamples, m_id);
		m_callback->writeCallback(output, nSamples, m_id);
	}
}

bool CSoundCard::convertNameToDevices(PaDeviceIndex& inDev, PaDeviceIndex& outDev)
{
	inDev = outDev = -1;

	PaHostApiIndex apiIndex = ::Pa_HostApiTypeIdToHostApiIndex(paALSA);
	if (apiIndex == paHostApiNotFound)
		return false;

	PaDeviceIndex n = ::Pa_GetDeviceCount();
	if (n <= 0)
		return false;

	for (PaDeviceIndex i = 0; i < n; i++) {
		const PaDeviceInfo* device = ::Pa_GetDeviceInfo(i);

		if (device->hostApi != apiIndex)
			continue;

		if (!m_readDevice.empty() && m_readDevice == device->name && device->maxInputChannels > 0)
			inDev = i;

		if (!m_writeDevice.empty() && m_writeDevice == device->name && device->maxOutputChannels > 0)
			outDev = i;
	}

	if (inDev == -1 && outDev == -1)
		return false;

	return true;
}
