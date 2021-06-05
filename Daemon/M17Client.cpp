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
#include "GitVersion.h"
#include "SoundCard.h"
#include "StopWatch.h"
#include "Version.h"
#include "Thread.h"
#include "Log.h"

#include <cstdio>
#include <vector>

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pwd.h>

const char* DEFAULT_INI_FILE = "/etc/M17Client.ini";

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

	::LogFinalise();

	return ret;
}

CM17Client::CM17Client(const std::string& confFile) :
m_conf(confFile),
m_codePlug(NULL),
m_rx(NULL),
m_tx(NULL)
{
}

CM17Client::~CM17Client()
{
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

	CSoundCard sound(m_conf.getAudioInputDevice(), m_conf.getAudioOutputDevice(), CODEC_SAMPLE_RATE, CODEC_BLOCK_SIZE);
	// XXX FIXME
	ret = sound.open();
	if (!ret) {
		LogError("Unable to open the sound card");
		return 1;
	}

	CRSSIInterpolator* interpolator = NULL;	// XXX FIXME

	m_tx = new CM17TX(m_conf.getCallsign(), m_conf.getText());
	m_rx = new CM17RX(m_conf.getCallsign(), interpolator);

	// By default use the first entry in the code plug file
	m_rx->setCAN(m_codePlug->getData().at(0U).m_can);
	m_tx->setCAN(m_codePlug->getData().at(0U).m_can);

	CStopWatch stopWatch;
	stopWatch.start();

	LogMessage("M17Client-%s is running", VERSION);

	while (!m_killed) {
		unsigned int ms = stopWatch.elapsed();
		stopWatch.start();

		if (ms < 10U)
			CThread::sleep(10U);
	}

	sound.close();

	delete m_codePlug;
	delete m_tx;
	delete m_rx;

	return 0;
}

