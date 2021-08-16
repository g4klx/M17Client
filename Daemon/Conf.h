/*
 *   Copyright (C) 2015,2016,2017,2019,2020,2021 by Jonathan Naylor G4KLX
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

#if !defined(CONF_H)
#define	CONF_H

#include <string>
#include <vector>

class CConf
{
public:
	CConf(const std::string& file);
	~CConf();

	bool read();

	// The General section
	std::string  getCallsign() const;
	std::string  getText() const;
	bool         getBleep() const;
	bool         getDaemon() const;
	bool         getDebug() const;

	// The Destinations sections
	std::vector<std::string> getDestinations() const;

	// The Audio section
	std::string  getAudioInputDevice() const;
	std::string  getAudioOutputDevice() const;
	unsigned int getAudioMicGain() const;
	unsigned int getAudioVolume() const;

	// The Modem section
	std::string  getModemPort() const;
	unsigned int getModemSpeed() const;
	bool         getModemRXInvert() const;
	bool         getModemTXInvert() const;
	bool         getModemPTTInvert() const;
	unsigned int getModemTXDelay() const;
	int          getModemTXOffset() const;
	int          getModemRXOffset() const;
	int          getModemRXDCOffset() const;
	int          getModemTXDCOffset() const;
	float        getModemRFLevel() const;
	float        getModemRXLevel() const;
	float        getModemTXLevel() const;
	std::string  getModemRSSIMappingFile() const;
	bool         getModemTrace() const;
	bool         getModemDebug() const;

	// The Log section
	unsigned int getLogDisplayLevel() const;
	unsigned int getLogFileLevel() const;
	std::string  getLogFilePath() const;
	std::string  getLogFileRoot() const;
	bool         getLogFileRotate() const;

	// The CodePlug section
	std::string  getCodePlugFile() const;

	// The GPIO section
	bool         getGPIOEnabled() const;
	bool         getGPIOPTTInvert() const;
	unsigned int getGPIOPTTPin() const;
	bool         getGPIOVolumeInvert() const;
	unsigned int getGPIOVolumeUpPin() const;
	unsigned int getGPIOVolumeDownPin() const;

	// The HamLib section
	bool	     getHamLibEnabled() const;
	std::string  getHamLibRadioType() const;
	std::string  getHamLibPort() const;
	unsigned int getHamLibSpeed() const;

	// The GPSD section
	bool         getGPSDEnabled() const;
	std::string  getGPSDAddress() const;
	std::string  getGPSDPort() const;

	// The Control section
	std::string    getControlRemoteAddress() const;
	unsigned short getControlRemotePort() const;
	std::string    getControlLocalAddress() const;
	unsigned short getControlLocalPort() const;

private:
	std::string  m_file;
	std::string  m_callsign;
	std::string  m_text;
	bool         m_bleep;
	bool         m_daemon;
	bool         m_debug;

	std::vector<std::string> m_destinations;

	std::string  m_audioInputDevice;
	std::string  m_audioOutputDevice;
	unsigned int m_audioMicGain;
	unsigned int m_audioVolume;

	std::string  m_modemPort;
	unsigned int m_modemSpeed;
	bool         m_modemRXInvert;
	bool         m_modemTXInvert;
	bool         m_modemPTTInvert;
	unsigned int m_modemTXDelay;
	int          m_modemTXOffset;
	int          m_modemRXOffset;
	int          m_modemRXDCOffset;
	int          m_modemTXDCOffset;
	float        m_modemRFLevel;
	float        m_modemRXLevel;
	float        m_modemTXLevel;
	std::string  m_modemRSSIMappingFile;
	bool         m_modemTrace;
	bool         m_modemDebug;

	unsigned int m_logDisplayLevel;
	unsigned int m_logFileLevel;
	std::string  m_logFilePath;
	std::string  m_logFileRoot;
	bool         m_logFileRotate;

	std::string  m_codePlugFile;

	bool         m_gpioEnabled;
	bool         m_gpioPTTInvert;
	unsigned int m_gpioPTTPin;
	bool         m_gpioVolumeInvert;
	unsigned int m_gpioVolumeUpPin;
	unsigned int m_gpioVolumeDownPin;

	bool         m_hamLibEnabled;
	std::string  m_hamLibRadioType;
	std::string  m_hamLibPort;
	unsigned int m_hamLibSpeed;

	bool         m_gpsdEnabled;
	std::string  m_gpsdAddress;
	std::string  m_gpsdPort;

	std::string    m_controlRemoteAddress;
	unsigned short m_controlRemotePort;
	std::string    m_controlLocalAddress;
	unsigned short m_controlLocalPort;
};

#endif
