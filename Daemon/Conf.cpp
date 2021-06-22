/*
 *   Copyright (C) 2015-2021 by Jonathan Naylor G4KLX
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

#include "Conf.h"
#include "Log.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

const int BUFFER_SIZE = 500;

enum SECTION {
	SECTION_NONE,
	SECTION_GENERAL,
	SECTION_DESTINATIONS,
	SECTION_AUDIO,
	SECTION_MODEM,
	SECTION_LOG,
	SECTION_CODE_PLUG,
	SECTION_GPIO,
	SECTION_HAMLIB,
	SECTION_GPSD,
	SECTION_CONTROL
};

CConf::CConf(const std::string& file) :
m_file(file),
m_callsign(),
m_text(),
m_bleep(true),
m_daemon(false),
m_debug(false),
m_destinations(),
m_audioInputDevice(),
m_audioOutputDevice(),
m_audioMicGain(100U),
m_audioVolume(100U),
m_modemPort(),
m_modemSpeed(460800U),
m_modemRXInvert(false),
m_modemTXInvert(false),
m_modemPTTInvert(false),
m_modemTXDelay(100U),
m_modemTXOffset(0),
m_modemRXOffset(0),
m_modemRXDCOffset(0),
m_modemTXDCOffset(0),
m_modemRFLevel(100.0F),
m_modemRXLevel(50.0F),
m_modemTXLevel(50.0F),
m_modemRSSIMappingFile(),
m_modemTrace(false),
m_modemDebug(false),
m_logDisplayLevel(0U),
m_logFileLevel(0U),
m_logFilePath(),
m_logFileRoot(),
m_logFileRotate(true),
m_codePlugFile("CodePlug.ini"),
m_gpioEnabled(false),
m_gpioPTTPin(15U),
m_hamLibEnabled(false),
m_hamLibRadioType(),
m_gpsdEnabled(false),
m_gpsdAddress("127.0.0.1"),
m_gpsdPort(),
m_controlRemoteAddress("127.0.0.1"),
m_controlRemotePort(0U),
m_controlLocalAddress("127.0.0.1"),
m_controlLocalPort(0U)
{
}

CConf::~CConf()
{
}

bool CConf::read()
{
	FILE* fp = ::fopen(m_file.c_str(), "rt");
	if (fp == NULL) {
		::fprintf(stderr, "Couldn't open the .ini file - %s\n", m_file.c_str());
		return false;
	}

	SECTION section = SECTION_NONE;

	char buffer[BUFFER_SIZE];
	while (::fgets(buffer, BUFFER_SIZE, fp) != NULL) {
		if (buffer[0U] == '#')
			continue;

		if (buffer[0U] == '[') {
			if (::strncmp(buffer, "[General]", 9U) == 0)
				section = SECTION_GENERAL;
			else if (::strncmp(buffer, "[Destinations]", 14U) == 0)
				section = SECTION_DESTINATIONS;
			else if (::strncmp(buffer, "[Audio]", 7U) == 0)
				section = SECTION_AUDIO;
			else if (::strncmp(buffer, "[Modem]", 7U) == 0)
				section = SECTION_MODEM;
			else if (::strncmp(buffer, "[Log]", 5U) == 0)
				section = SECTION_LOG;
			else if (::strncmp(buffer, "[Code Plug]", 11U) == 0)
				section = SECTION_CODE_PLUG;
			else if (::strncmp(buffer, "[GPIO]", 6U) == 0)
				section = SECTION_GPIO;
			else if (::strncmp(buffer, "[HamLib]", 8U) == 0)
				section = SECTION_HAMLIB;
			else if (::strncmp(buffer, "[GPSD]", 6U) == 0)
				section = SECTION_GPSD;
			else if (::strncmp(buffer, "[Control]", 9U) == 0)
				section = SECTION_CONTROL;
			else
				section = SECTION_NONE;

			continue;
		}

		char* key   = ::strtok(buffer, " \t=\r\n");
		if (key == NULL)
			continue;

		char* value = ::strtok(NULL, "\r\n");
		if (value == NULL)
			continue;

		// Remove quotes from the value
		size_t len = ::strlen(value);
		if (len > 1U && *value == '"' && value[len - 1U] == '"') {
			value[len - 1U] = '\0';
			value++;
		} else {
			// if value is not quoted, remove after # (to make comment)
			(void)::strtok(value, "#");
		}

		if (section == SECTION_GENERAL) {
			if (::strcmp(key, "Callsign") == 0) {
				// Convert the callsign to upper case
				for (unsigned int i = 0U; value[i] != 0; i++)
					value[i] = ::toupper(value[i]);
				m_callsign = value;
			} else if (::strcmp(key, "Text") == 0)
				m_text = value;
			else if (::strcmp(key, "Bleep") == 0)
				m_bleep = ::atoi(value) == 1;
			else if (::strcmp(key, "Daemon") == 0)
				m_daemon = ::atoi(value) == 1;
			else if (::strcmp(key, "Debug") == 0)
				m_debug = ::atoi(value) == 1;
		} else if (section == SECTION_DESTINATIONS) {
			if (::strcmp(key, "Name") == 0) {
				for (unsigned int i = 0U; value[i] != 0; i++)
					value[i] = ::toupper(value[i]);
				m_destinations.push_back(value);
			}
		} else if (section == SECTION_AUDIO) {
			if (::strcmp(key, "InputDevice") == 0)
				m_audioInputDevice = value;
			else if (::strcmp(key, "OutputDevice") == 0)
				m_audioOutputDevice = value;
			else if (::strcmp(key, "MicGain") == 0)
				m_audioMicGain = (unsigned int)::atoi(value);
			else if (::strcmp(key, "Volume") == 0)
				m_audioVolume = (unsigned int)::atoi(value);
		} else if (section == SECTION_MODEM) {
			if (::strcmp(key, "Port") == 0)
				m_modemPort = value;
			else if (::strcmp(key, "Speed") == 0)
				m_modemSpeed = (unsigned int)::atoi(value);
			else if (::strcmp(key, "RXInvert") == 0)
				m_modemRXInvert = ::atoi(value) == 1;
			else if (::strcmp(key, "TXInvert") == 0)
				m_modemTXInvert = ::atoi(value) == 1;
			else if (::strcmp(key, "PTTInvert") == 0)
				m_modemPTTInvert = ::atoi(value) == 1;
			else if (::strcmp(key, "TXDelay") == 0)
				m_modemTXDelay = (unsigned int)::atoi(value);
			else if (::strcmp(key, "RXOffset") == 0)
				m_modemRXOffset = ::atoi(value);
			else if (::strcmp(key, "TXOffset") == 0)
				m_modemTXOffset = ::atoi(value);
			else if (::strcmp(key, "RXDCOffset") == 0)
				m_modemRXDCOffset = ::atoi(value);
			else if (::strcmp(key, "TXDCOffset") == 0)
				m_modemTXDCOffset = ::atoi(value);
			else if (::strcmp(key, "RFLevel") == 0)
				m_modemRFLevel = float(::atof(value));
			else if (::strcmp(key, "RXLevel") == 0)
				m_modemRXLevel = float(::atof(value));
			else if (::strcmp(key, "TXLevel") == 0)
				m_modemTXLevel = float(::atof(value));
			else if (::strcmp(key, "RSSIMappingFile") == 0)
				m_modemRSSIMappingFile = value;
			else if (::strcmp(key, "Trace") == 0)
				m_modemTrace = ::atoi(value) == 1;
			else if (::strcmp(key, "Debug") == 0)
				m_modemDebug = ::atoi(value) == 1;
		} else if (section == SECTION_LOG) {
			if (::strcmp(key, "FilePath") == 0)
				m_logFilePath = value;
			else if (::strcmp(key, "FileRoot") == 0)
				m_logFileRoot = value;
			else if (::strcmp(key, "FileLevel") == 0)
				m_logFileLevel = (unsigned int)::atoi(value);
			else if (::strcmp(key, "DisplayLevel") == 0)
				m_logDisplayLevel = (unsigned int)::atoi(value);
			else if (::strcmp(key, "FileRotate") == 0)
				m_logFileRotate = ::atoi(value) == 1;
		} else if (section == SECTION_CODE_PLUG) {
			if (::strcmp(key, "File") == 0)
				m_codePlugFile = value;
		} else if (section == SECTION_GPIO) {
			if (::strcmp(key, "Enable") == 0)
				m_gpioEnabled = ::atoi(value) == 1;
			else if (::strcmp(key, "PTT") == 0)
				m_gpioPTTPin = (unsigned int)::atoi(value);
		} else if (section == SECTION_HAMLIB) {
			if (::strcmp(key, "Enable") == 0)
				m_hamLibEnabled = ::atoi(value) == 1;
			else if (::strcmp(key, "RadioType") == 0)
				m_hamLibRadioType = value;
		} else if (section == SECTION_GPSD) {
			if (::strcmp(key, "Enable") == 0)
				m_gpsdEnabled = ::atoi(value) == 1;
			else if (::strcmp(key, "Address") == 0)
				m_gpsdAddress = value;
			else if (::strcmp(key, "Port") == 0)
				m_gpsdPort = value;
		} else if (section == SECTION_CONTROL) {
			if (::strcmp(key, "RemoteAddress") == 0)
				m_controlRemoteAddress = value;
			else if (::strcmp(key, "RemotePort") == 0)
				m_controlRemotePort = (unsigned short)::atoi(value);
			else if (::strcmp(key, "LocalAddress") == 0)
				m_controlLocalAddress = value;
			else if (::strcmp(key, "LocalPort") == 0)
				m_controlLocalPort = (unsigned short)::atoi(value);
		}
	}

	::fclose(fp);

	return true;
}

std::string CConf::getCallsign() const
{
	return m_callsign;
}

std::string CConf::getText() const
{
	return m_text;
}

bool CConf::getBleep() const
{
	return m_bleep;
}

bool CConf::getDaemon() const
{
	return m_daemon;
}

bool CConf::getDebug() const
{
	return m_debug;
}

std::vector<std::string> CConf::getDestinations() const
{
	return m_destinations;
}

std::string CConf::getAudioInputDevice() const
{
	return m_audioInputDevice;
}

std::string CConf::getAudioOutputDevice() const
{
	return m_audioOutputDevice;
}

unsigned int CConf::getAudioMicGain() const
{
	return m_audioMicGain;
}

unsigned int CConf::getAudioVolume() const
{
	return m_audioVolume;
}

std::string CConf::getModemPort() const
{
	return m_modemPort;
}

unsigned int CConf::getModemSpeed() const
{
	return m_modemSpeed;
}

bool CConf::getModemRXInvert() const
{
	return m_modemRXInvert;
}

bool CConf::getModemTXInvert() const
{
	return m_modemTXInvert;
}

bool CConf::getModemPTTInvert() const
{
	return m_modemPTTInvert;
}

unsigned int CConf::getModemTXDelay() const
{
	return m_modemTXDelay;
}

int CConf::getModemRXOffset() const
{
	return m_modemRXOffset;
}

int CConf::getModemTXOffset() const
{
	return m_modemTXOffset;
}

int CConf::getModemRXDCOffset() const
{
	return m_modemRXDCOffset;
}

int CConf::getModemTXDCOffset() const
{
	return m_modemTXDCOffset;
}

float CConf::getModemRFLevel() const
{
	return m_modemRFLevel;
}

float CConf::getModemRXLevel() const
{
	return m_modemRXLevel;
}

float CConf::getModemTXLevel() const
{
	return m_modemTXLevel;
}

std::string CConf::getModemRSSIMappingFile () const
{
	return m_modemRSSIMappingFile;
}

bool CConf::getModemTrace() const
{
	return m_modemTrace;
}

bool CConf::getModemDebug() const
{
	return m_modemDebug;
}

unsigned int CConf::getLogDisplayLevel() const
{
	return m_logDisplayLevel;
}

unsigned int CConf::getLogFileLevel() const
{
	return m_logFileLevel;
}

std::string CConf::getLogFilePath() const
{
	return m_logFilePath;
}

std::string CConf::getLogFileRoot() const
{
	return m_logFileRoot;
}

bool CConf::getLogFileRotate() const
{
	return m_logFileRotate;
}

std::string CConf::getCodePlugFile() const
{
	return m_codePlugFile;
}

bool CConf::getGPIOEnabled() const
{
	return m_gpioEnabled;
}

unsigned int CConf::getGPIOPTTPin() const
{
	return m_gpioPTTPin;
}

bool CConf::getHamLibEnabled() const
{
	return m_hamLibEnabled;
}

std::string CConf::getHamLibRadioType() const
{
	return m_hamLibRadioType;
}

bool CConf::getGPSDEnabled() const
{
	return m_gpsdEnabled;
}

std::string CConf::getGPSDAddress() const
{
	return m_gpsdAddress;
}

std::string CConf::getGPSDPort() const
{
	return m_gpsdPort;
}

std::string CConf::getControlRemoteAddress() const
{
	return m_controlRemoteAddress;
}

unsigned short CConf::getControlRemotePort() const
{
	return m_controlRemotePort;
}

std::string CConf::getControlLocalAddress() const
{
	return m_controlLocalAddress;
}

unsigned short CConf::getControlLocalPort() const
{
	return m_controlLocalPort;
}

