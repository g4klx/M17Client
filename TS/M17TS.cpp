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

#include "M17TS.h"
#include "GitVersion.h"
#include "Version.h"
#include "Thread.h"
#include "Timer.h"
#include "Log.h"

#include <cassert>
#include <cstring>
#include <cstdio>

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pwd.h>

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
	if (argc > 1) {
		for (int currentArg = 1; currentArg < argc; ++currentArg) {
			std::string arg = argv[currentArg];
			if ((arg == "-v") || (arg == "--version")) {
				::fprintf(stdout, "M17TS version %s git #%.7s\n", VERSION, gitversion);
				return 0;
			} else if (arg.substr(0,1) == "-") {
				::fprintf(stderr, "Usage: M17TS [-v|--version]\n");
				return 1;
			}
		}
	}

	::signal(SIGINT,  sigHandler);
	::signal(SIGTERM, sigHandler);
	::signal(SIGHUP,  sigHandler);

	int ret = 0;

	do {
		m_signal = 0;

		CM17TS* host = new CM17TS;
		ret = host->run();

		delete host;

		if (m_signal == 2)
			::LogInfo("M17TS-%s exited on receipt of SIGINT", VERSION);

		if (m_signal == 15)
			::LogInfo("M17TS-%s exited on receipt of SIGTERM", VERSION);

		if (m_signal == 1)
			::LogInfo("M17TS-%s restarted on receipt of SIGHUP", VERSION);
	} while (m_signal == 1);

	::LogFinalise();

	return ret;
}

CM17TS::CM17TS() :
m_conf(),
m_socket(NULL),
m_uart(NULL),
m_sockaddr(),
m_sockaddrLen(0U),
m_channels(),
m_destinations(),
m_modules()
{
}

CM17TS::~CM17TS()
{
}

int CM17TS::run()
{
	bool ret = m_conf.read();
	if (!ret) {
		::fprintf(stderr, "M17TS: cannot read the configuration file\n");
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

        ret = ::LogInitialise(m_daemon, ".", "M17TS", 1U, 1U, true);
	if (!ret) {
		::fprintf(stderr, "M17TS: unable to open the log file\n");
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

	LogMessage("M17TS-%s is starting", VERSION);
	LogMessage("Built %s %s (GitID #%.7s)", __TIME__, __DATE__, gitversion);

	m_uart = new CUARTController(m_conf.getScreenPort(), m_conf.getScreenSpeed());
	ret = m_uart->open();
	if (!ret) {
		delete m_uart;
		LogError("Unable to open the screen");
		::LogFinalise();
		return 1;
	}

	if (CUDPSocket::lookup(m_conf.getDaemonAddress(), m_conf.getDaemonPort(), m_sockaddr, m_sockaddrLen) != 0) {
		LogError("Could not lookup the daemon address");
		::LogFinalise();
		return 1;
	}

	m_socket = new CUDPSocket(m_conf.getSelfAddress(), m_conf.getSelfPort());
	ret = m_socket->open();
	if (!ret) {
		delete m_socket;
		LogError("Unable to open the control socket");
		::LogFinalise();
		return 1;
	}

	LogMessage("M17TS-%s is running", VERSION);

	CTimer timer(1000U, 0U, 100U);
	timer.start();

	while (!m_killed) {
		char command[100U];

		sockaddr_storage sockaddr;
		unsigned int sockaddrLen = 0U;
		int ret = m_socket->read(command, 100U, sockaddr, sockaddrLen);
		if (ret > 0) {
			command[ret] = '\0';
			parseCommand(command);
		}

		ret = m_uart->read(command, 100U);
		if (ret > 0) {
			command[ret] = '\0';
			parseScreen(command);
		}

		timer.clock(20U);
		if (timer.isRunning() && timer.hasExpired()) {
			if (m_channels.empty()) {
				getChannels();
				timer.start();
			} else if (m_channels.empty()) {
				getDestinations();
				timer.start();
			} else {
				timer.stop();
			}
		}

		CThread::sleep(20U);
	}

	m_conf.write();

	m_socket->close();
	m_uart->close();

	delete m_uart;
	delete m_socket;

	::LogFinalise();

	return 0;
}

void CM17TS::parseCommand(char* command)
{
	assert(command != NULL);

	std::vector<char *> ptrs;

	char* s = command;
	char* p;
	while ((p = ::strtok(s, DELIMITER)) != NULL) {
		s = NULL;
		ptrs.push_back(p);
	}

	if (::strcmp(ptrs.at(0U), "CHAN") == 0) {
		m_channels.clear();
		for (unsigned int i = 1U; i < ptrs.size(); i++) {
			std::string channel = std::string(ptrs.at(i));
			m_channels.push_back(channel);
		}
	} else if (::strcmp(ptrs.at(0U), "DEST") == 0) {
		m_destinations.clear();
		for (unsigned int i = 1U; i < ptrs.size(); i++) {
			std::string destination = std::string(ptrs.at(i));
			m_destinations.push_back(destination);
		}
	} else if (::strcmp(ptrs.at(0U), "RX") == 0) {
		bool end                = ::atoi(ptrs.at(1U)) == 1;
		std::string source      = std::string(ptrs.at(2U));
		std::string destination = std::string(ptrs.at(3U));
	} else if (::strcmp(ptrs.at(0U), "TEXT") == 0) {
		std::string text = std::string(ptrs.at(1U));
	} else if (::strcmp(ptrs.at(0U), "RSSI") == 0) {
		int rssi = ::atoi(ptrs.at(1U));
	}
}

void CM17TS::parseScreen(char* command)
{
	assert(command != NULL);
}

bool CM17TS::getChannels()
{
	assert(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "CHAN");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, "?");

	return m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

bool CM17TS::setChannel(const std::string& channel)
{
	assert(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "CHAN");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, channel.c_str());

	return m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

bool CM17TS::getDestinations()
{
	assert(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "DEST");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, "?");

	return m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

bool CM17TS::setDestination(const std::string& destination)
{
	assert(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "DEST");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, destination.c_str());

	return m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

bool CM17TS::setVolume(unsigned int volume)
{
	assert(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "VOL");
	::strcat(buffer, DELIMITER);
	::sprintf(buffer + ::strlen(buffer), "%u", volume);

	return m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

bool CM17TS::setMicGain(unsigned int micGain)
{
	assert(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "MIC");
	::strcat(buffer, DELIMITER);
	::sprintf(buffer + ::strlen(buffer), "%u", micGain);

	return m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

bool CM17TS::setTransmit(bool transmit)
{
	assert(m_socket != NULL);

	char buffer[20U];
	::strcpy(buffer, "TX");
	::strcat(buffer, DELIMITER);
	::strcat(buffer, transmit ? "1" : "0");

	return m_socket->write(buffer, ::strlen(buffer), m_sockaddr, m_sockaddrLen);
}

