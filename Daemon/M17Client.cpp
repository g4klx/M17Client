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

#include "M17Client.h"
#include "UARTController.h"
#include "codec2/codec2.h"
#include "GitVersion.h"
#include "UDPSocket.h"
#include "SoundCard.h"
#include "StopWatch.h"
#include "Version.h"
#include "Thread.h"
#include "Modem.h"
#include "Log.h"

#include <cstdio>
#include <vector>

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pwd.h>

const char* DEFAULT_INI_FILE = "/etc/M17Client.ini";

const char* DELIMITER = ":";

static bool m_killed = false;
static int  m_signal = 0;

static void sigHandler(int signum)
{
	m_killed = true;
	m_signal = signum;
}

const char* HEADER1 = "This software is for use on amateur radio networks only,";
const char* HEADER2 = "it is to be used for educational purposes only. Its use on";
const char* HEADER3 = "commercial networks is strictly prohibited.";
const char* HEADER4 = "Copyright(C) 2021 by Jonathan Naylor, G4KLX and others";

int main(int argc, char** argv)
{
	const char* iniFile = DEFAULT_INI_FILE;

	if (argc > 1) {
		for (int currentArg = 1; currentArg < argc; ++currentArg) {
			std::string arg = argv[currentArg];
			if ((arg == "-v") || (arg == "--version")) {
				::fprintf(stdout, "M17Client version %s git #%.7s\n", VERSION, gitversion);
				return 0;
			} else if (arg.substr(0,1) == "-") {
				::fprintf(stderr, "Usage: M17Client [-v|--version] [filename]\n");
				return 1;
			} else {
				iniFile = argv[currentArg];
			}
		}
	}

	::signal(SIGINT,  sigHandler);
	::signal(SIGTERM, sigHandler);
	::signal(SIGHUP,  sigHandler);

	int ret = 0;

	do {
		m_signal = 0;

		CM17Client* host = new CM17Client(std::string(iniFile));
		ret = host->run();

		delete host;

		if (m_signal == 2)
			::LogInfo("M17Client-%s exited on receipt of SIGINT", VERSION);

		if (m_signal == 15)
			::LogInfo("M17Client-%s exited on receipt of SIGTERM", VERSION);

		if (m_signal == 1)
			::LogInfo("M17Client-%s restarted on receipt of SIGHUP", VERSION);
	} while (m_signal == 1);

	return ret;
}

CM17Client::CM17Client(const std::string& confFile) :
m_conf(confFile),
m_codePlug(NULL),
m_rx(NULL),
m_tx(NULL),
m_tx1(false),
m_tx2(false),
m_socket(NULL),
#if defined(USE_HAMLIB)
m_hamLib(NULL),
#endif
#if defined(USE_GPSD)
m_gpsd(NULL),
#endif
#if defined(USE_GPIO)
m_gpio(NULL),
#endif
m_sockaddr(),
m_sockaddrLen(0U)
{
}

CM17Client::~CM17Client()
{
}

void CM17Client::readCallback(const float* input, unsigned int nSamples, int id)
{
	assert(m_tx != NULL);

	if (nSamples > 0U)
		m_tx->write(input, nSamples);
}

void CM17Client::writeCallback(float* output, int& nSamples, int id)
{
	assert(m_rx != NULL);

	if (nSamples > 0)
		nSamples = int(m_rx->read(output, nSamples));
}

int CM17Client::run()
{
	bool ret = m_conf.read();
	if (!ret) {
		::fprintf(stderr, "M17Client: cannot read the .ini file\n");
		return 1;
	}

	m_codePlug = new CCodePlug(m_conf.getCodePlugFile());
	ret = m_codePlug->read();
	if (!ret) {
		::fprintf(stderr, "M17Client: cannot read the code plug file\n");
		return 1;
	}

	bool m_daemon = m_conf.getDaemon();
	if (m_daemon) {
		// Create new process
		pid_t pid = ::fork();
		if (pid == -1) {
			::fprintf(stderr, "Couldn't fork() , exiting\n");
			return -1;
		} else if (pid != 0) {
			exit(EXIT_SUCCESS);
		}

		// Create new session and process group
		if (::setsid() == -1){
			::fprintf(stderr, "Couldn't setsid(), exiting\n");
			return -1;
		}

		// Set the working directory to the root directory
		if (::chdir("/") == -1){
			::fprintf(stderr, "Couldn't cd /, exiting\n");
			return -1;
		}

		// If we are currently root...
		if (getuid() == 0) {
			struct passwd* user = ::getpwnam("m17");
			if (user == NULL) {
				::fprintf(stderr, "Could not get the m17 user, exiting\n");
				return -1;
			}
			
			uid_t mmdvm_uid = user->pw_uid;
			gid_t mmdvm_gid = user->pw_gid;

			// Set user and group ID's to m17:m17
			if (setgid(mmdvm_gid) != 0) {
				::fprintf(stderr, "Could not set m17 GID, exiting\n");
				return -1;
			}

			if (setuid(mmdvm_uid) != 0) {
				::fprintf(stderr, "Could not set m17 UID, exiting\n");
				return -1;
			}
		    
			// Double check it worked (AKA Paranoia) 
			if (setuid(0) != -1) {
				::fprintf(stderr, "It's possible to regain root - something is wrong!, exiting\n");
				return -1;
			}
		}
	}

	ret = ::LogInitialise(m_daemon, m_conf.getLogFilePath(), m_conf.getLogFileRoot(), m_conf.getLogFileLevel(), m_conf.getLogDisplayLevel(), m_conf.getLogFileRotate());
	if (!ret) {
		::fprintf(stderr, "M17Client: unable to open the log file\n");
		return 1;
	}

	if (m_daemon) {
		::close(STDIN_FILENO);
		::close(STDOUT_FILENO);
		::close(STDERR_FILENO);
	}

	LogInfo(HEADER1);
	LogInfo(HEADER2);
	LogInfo(HEADER3);
	LogInfo(HEADER4);

	LogMessage("M17Client-%s is starting", VERSION);
	LogMessage("Built %s %s (GitID #%.7s)", __TIME__, __DATE__, gitversion);

	CModem modem(false, m_conf.getModemRXInvert(), m_conf.getModemTXInvert(), m_conf.getModemPTTInvert(), m_conf.getModemTXDelay(),
		     0U, false, m_conf.getModemTrace(), m_conf.getModemDebug());

	modem.setPort(new CUARTController(m_conf.getModemPort(), m_conf.getModemSpeed()));

	// By default use the first entry in the code plug file
	modem.setRFParams(m_codePlug->getData().at(0U).m_rxFrequency, m_conf.getModemRXOffset(),
			  m_codePlug->getData().at(0U).m_txFrequency, m_conf.getModemTXOffset(),
			  m_conf.getModemTXDCOffset(), m_conf.getModemRXDCOffset(), m_conf.getModemRFLevel(), 0U);

	// Only enable M17
	modem.setModeParams(false, false, false, false, false, true, false, false, false, MODE_M17);

	// Only set the TX level for M17
	modem.setLevels(m_conf.getModemRXLevel(), 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, m_conf.getModemTXLevel(), 0.0F, 0.0F, 0.0F);

	// Set the M17 TX hang time to 0
	modem.setM17Params(0U);

	ret = modem.open();
	if (!ret) {
		LogError("Unable to open the MMDVM");
		::LogFinalise();
		return 1;
	}

	if (!modem.hasM17()) {
		LogError("Modem is not capable of M17");
		::LogFinalise();
		return 1;
	}

	if (CUDPSocket::lookup(m_conf.getControlRemoteAddress(), m_conf.getControlRemotePort(), m_sockaddr, m_sockaddrLen) != 0) {
		LogError("Could not lookup the remote address");
		::LogFinalise();
		return 1;
	}

	m_socket = new CUDPSocket(m_conf.getControlLocalAddress(), m_conf.getControlLocalPort());
	ret = m_socket->open();
	if (!ret) {
		LogError("Unable to open the command socket");
		::LogFinalise();
		return 1;
	}

#if defined(USE_HAMLIB)
	if (m_conf.getHamLibEnabled()) {
		m_hamLib = new CHamLib(m_conf.getHamLibRadioType(), m_conf.getHamLibPort(), m_conf.getHamLibSpeed());
		ret = m_hamLib->open();
		if (!ret) {
			LogError("Unable to open HamLib");
			::LogFinalise();
			return 1;
		}
	}
#endif

#if defined(USE_GPSD)
	if (m_conf.getGPSDEnabled()) {
		m_gpsd = new CGPSD(m_conf.getGPSDAddress(), m_conf.getGPSDPort());
		ret = m_gpsd->open();
		if (!ret) {
			LogError("Unable to open GPSD");
			::LogFinalise();
			return 1;
		}
	}
#endif

#if defined(USE_GPIO)
	if (m_conf.getGPIOEnabled()) {
		m_gpio = new CGPIO(m_conf.getGPIOPTTInvert(), m_conf.getGPIOPTTPin(), m_conf.getGPIOVolumeInvert(), m_conf.getGPIOVolumeUpPin(), m_conf.getGPIOVolumeDownPin());
		ret = m_gpio->open();
		if (!ret) {
			LogError("Unable to open GPIO");
			::LogFinalise();
			return 1;
		}
	}
#endif

	CCodec2 codec3200(true);
	CCodec2 codec1600(false);

	CRSSIInterpolator* rssi = new CRSSIInterpolator;
	if (!m_conf.getModemRSSIMappingFile().empty())
		rssi->load(m_conf.getModemRSSIMappingFile());

	m_tx = new CM17TX(m_conf.getCallsign(), m_conf.getText(), codec3200, codec1600);
	m_tx->setMicGain(m_conf.getAudioMicGain());
	m_tx->setDestination("ALL");

	m_rx = new CM17RX(m_conf.getCallsign(), rssi, m_conf.getBleep(), codec3200, codec1600);
	m_rx->setVolume(m_conf.getAudioVolume());
	m_rx->setStatusCallback(this);

	// By default use the first entry in the code plug file
#if defined(USE_HAMLIB)
	if (m_hamLib != NULL) {
		m_hamLib->setFrequency(m_codePlug->getData().at(0U).m_rxFrequency,
					m_codePlug->getData().at(0U).m_txFrequency);
	}
#endif
	m_tx->setCAN(m_codePlug->getData().at(0U).m_can);

	CSoundCard sound(m_conf.getAudioInputDevice(), m_conf.getAudioOutputDevice(), SOUNDCARD_SAMPLE_RATE, SOUNDCARD_BLOCK_SIZE);
	sound.setCallback(this);
	ret = sound.open();
	if (!ret) {
		LogError("Unable to open the sound card");
		::LogFinalise();
		return 1;
	}

	CStopWatch stopWatch;
	stopWatch.start();

	LogMessage("M17Client-%s is running", VERSION);

	while (!m_killed) {
		m_tx->process();

		bool tx = false;
		if (modem.hasM17Space()) {
			unsigned char data[M17_FRAME_LENGTH_BYTES];
			unsigned int len = m_tx->read(data);
			if (len > 0U) {
				modem.writeM17Data(data, len);
				tx = true;
			}
		}

		if (!tx) {
			unsigned char data[M17_FRAME_LENGTH_BYTES];
			unsigned int len = modem.readM17Data(data);
			if (len > 0U)
				m_rx->write(data, len);
		}

		char command[100U];
		sockaddr_storage sockaddr;
		unsigned int sockaddrLen = 0U;
		int ret = m_socket->read(command, 100U, sockaddr, sockaddrLen);
		if (ret > 0) {
			command[ret] = '\0';
			parseCommand(command);
		}

#if defined(USE_GPIO)
		if (m_gpio != NULL) {
			bool tx = m_gpio->getPTT();

			if (tx && !m_tx1 && !m_tx2) {
				LogDebug("\tTransmitter on");
				m_tx->start();
				sendTX(true);
			} else if (!tx && !m_tx1 && m_tx2) {
				LogDebug("\tTransmitter off");
				m_tx->end();
				sendTX(false);
			}

			m_tx2 = tx;

			bool volumeUp   = m_gpio->getVolumeUp();
			bool volumeDown = m_gpio->getVolumeDown();

			if (volumeUp || volumeDown) {
				unsigned int volume = m_rx->getVolume();
			
				if (volumeUp) {
					if (volume < 500U)
						volume++;
				} else if (volumeDown) {
					if (volume > 0U)
						volume--;
				}
			
				LogDebug("Volume set to %u", volume);

				m_rx->setVolume(volume);
			}
		}
#endif

		unsigned int ms = stopWatch.elapsed();
		stopWatch.start();

		modem.clock(ms);

		if (ms < 10U)
			CThread::sleep(10U);
	}

#if defined(USE_HAMLIB)
	if (m_hamLib != NULL) {
		m_hamLib->close();
		delete m_hamLib;
	}
#endif

#if defined(USE_GPSD)
	if (m_gpsd != NULL) {
		m_gpsd->close();
		delete m_gpsd;
	}
#endif

#if defined(USE_GPIO)
	if (m_gpio != NULL) {
		m_gpio->close();
		delete m_gpio;
	}
#endif

	m_socket->close();
	modem.close();
	sound.close();

	delete m_codePlug;
	delete m_tx;
	delete m_rx;
	delete m_socket;

	::LogFinalise();

	return 0;
}

void CM17Client::parseCommand(char* command)
{
	assert(command != NULL);
	assert(m_tx != NULL);
	assert(m_rx != NULL);

	LogDebug("Command received: %s", command);

	std::vector<char *> ptrs;

	char* s = command;
	char* p;
	while ((p = ::strtok(s, DELIMITER)) != NULL) {
		s = NULL;
		ptrs.push_back(p);
	}

	if (::strcmp(ptrs.at(0U), "TX") == 0) {
		if (::strcmp(ptrs.at(1U), "0") == 0) {
			if (m_tx1 && !m_tx2) {
				LogDebug("\tTransmitter off");
				m_tx->end();
				sendTX(false);
			}
			m_tx1 = false;
		} else if (::strcmp(ptrs.at(1U), "1") == 0) {
			if (!m_tx1 && !m_tx2) {
				LogDebug("\tTransmitter on");
				m_tx->start();
				sendTX(true);
			}
			m_tx1 = true;
		} else {
			LogWarning("\tUnknown TX command");
		}
	} else if (::strcmp(ptrs.at(0U), "CHAN") == 0) {
		if (::strcmp(ptrs.at(1U), "?") == 0) {
			LogDebug("\tChannel list request");
			sendChannelList();
		} else {
			LogDebug("\tChannel set to \"%s\"", ptrs.at(1U));
			bool ret = processChannelRequest(ptrs.at(1U));
			if (!ret)
				LogWarning("\tInvalid channel request");
		}
	} else if (::strcmp(ptrs.at(0U), "DEST") == 0) {
		if (::strcmp(ptrs.at(1U), "?") == 0) {
			LogDebug("\tDestination list request");
			sendDestinationList();
		} else {
			LogDebug("\tDestination set to \"%s\"", ptrs.at(1U));
			m_tx->setDestination(ptrs.at(1U));
		}
	} else if (::strcmp(ptrs.at(0U), "VOL") == 0) {
		LogDebug("\tVolume set to %s", ptrs.at(1U));
		m_rx->setVolume(::atoi(ptrs.at(1U)));
	} else if (::strcmp(ptrs.at(0U), "MIC") == 0) {
		LogDebug("\tMic gain set to %s", ptrs.at(1U));
		m_tx->setMicGain(::atoi(ptrs.at(1U)));
	} else {
		LogWarning("\tUnknown command");
	}
}

void CM17Client::sendChannelList()
{
	assert(m_codePlug != NULL);
	assert(m_socket != NULL);

	char buffer[1000U];
	::strcpy(buffer, "CHAN");

	for (const auto& chan : m_codePlug->getData()) {
		::strcat(buffer, DELIMITER);
		::strcat(buffer, chan.m_name.c_str());		
	}

	m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

bool CM17Client::processChannelRequest(const char* channel)
{
	assert(channel != NULL);
	assert(m_tx != NULL);
	assert(m_rx != NULL);

	for (const auto& chan : m_codePlug->getData()) {
		if (chan.m_name == channel) {
#if defined(USE_HAMLIB)
			if (m_hamLib != NULL)
				m_hamLib->setFrequency(chan.m_rxFrequency, chan.m_txFrequency);
#endif
			m_tx->setCAN(chan.m_can);
			return true;
		}
	}

	return false;
}

void CM17Client::sendTX(bool tx)
{
	assert(m_socket != NULL);

	char buffer[10U];
	::strcpy(buffer, "TX");
	::strcat(buffer, DELIMITER);

	if (tx)
		::strcat(buffer, "1");
	else
		::strcat(buffer, "0");

	m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

void CM17Client::sendDestinationList()
{
	assert(m_socket != NULL);

	char buffer[1000U];
	::strcpy(buffer, "DEST");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, "ALL");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, "INFO");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, "ECHO");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, "UNLINK");

	for (const auto& dest : m_conf.getDestinations()) {
		::strcat(buffer, DELIMITER);
		::strcat(buffer, dest.c_str());		
	}

	m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

void CM17Client::statusCallback(const std::string& source, const std::string& dest, bool end)
{
	assert(m_socket != NULL);

	char buffer[50U];
	::strcpy(buffer, "RX");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, end ? "1" : "0");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, source.c_str());
	::strcat(buffer, DELIMITER);
	::strcat(buffer, dest.c_str());

	m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

void CM17Client::textCallback(const char* text)
{
	assert(m_socket != NULL);

	char buffer[50U];
	::strcpy(buffer, "TEXT");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, text);

	m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

void CM17Client::rssiCallback(int rssi)
{
	assert(m_socket != NULL);

	char buffer[50U];
	::strcpy(buffer, "RSSI");
	::strcat(buffer, DELIMITER);
	::sprintf(buffer + ::strlen(buffer), "%d", rssi);

	m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

void CM17Client::gpsCallback()
{
	assert(m_socket != NULL);
}

