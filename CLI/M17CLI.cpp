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

#include "UDPSocket.h"
#include "Thread.h"
#include "Log.h"

#include <cstring>

const char* DELIMITER = ":";

const char* HEADER1 = "This software is for use on amateur radio networks only,";
const char* HEADER2 = "it is to be used for educational purposes only. Its use on";
const char* HEADER3 = "commercial networks is strictly prohibited.";
const char* HEADER4 = "Copyright(C) 2021 by Jonathan Naylor, G4KLX and others";

int main(int argc, char** argv)
{
	if (argc < 3) {
		::fprintf(stderr, "Usage: M17CLI <local port> <remote port>\n");
		return 1;
	}
	
        bool ret = ::LogInitialise(false, ".", "M17CLI", 1U, 1U, true);
	if (!ret) {
		::fprintf(stderr, "M17CLI: Unable to open the log file\n");
		return 1;
	}

	LogInfo(HEADER1);
	LogInfo(HEADER2);
	LogInfo(HEADER3);
	LogInfo(HEADER4);

	LogMessage("M17 CLI is starting");

	sockaddr_storage sockaddr;
	unsigned int sockaddrLen = 0U;

	if (CUDPSocket::lookup("127.0.0.1", ::atoi(argv[2]), sockaddr, sockaddrLen) != 0) {
		LogError("Could not lookup the remote address");
		return 1;
	}

	CUDPSocket socket("127.0.0.1", ::atoi(argv[1]));
	ret = socket.open();
	if (!ret) {
		LogError("Unable to open the command socket");
		return 1;
	}

	char buffer[1000U];
	for (;;) {
		fd_set fd_set;
		FD_ZERO(&fd_set);
		FD_SET(STDIN_FILENO, &fd_set);

		timeval timeval;
		timeval.tv_sec  = 0;
		timeval.tv_usec = 0;

		int n = ::select(STDIN_FILENO + 1, &fd_set, NULL, NULL, &timeval);
		if (n > 0) {
			if (::fgets(buffer, 1000U, stdin) == NULL)
				break;

			char* p = ::strchr(buffer, '\n');
			if (p != NULL)
				*p = '\0';
			p = ::strchr(buffer, '\r');
			if (p != NULL)
				*p = '\0';

			LogMessage("TX: %s", buffer);

			socket.write(buffer, ::strlen(buffer), sockaddr, sockaddrLen);
		}

		sockaddr_storage sockaddr1;
		unsigned int sockaddrLen1 = 0U;
		n = socket.read(buffer, 1000U, sockaddr1, sockaddrLen1);
		if (n > 0) {
			buffer[n] = '\0';
			LogMessage("RX: %s", buffer);
		}
		
		CThread::sleep(20U);
	}

	LogMessage("M17 CLI is ending");

	socket.close();
	::LogFinalise();

	return 0;
}

