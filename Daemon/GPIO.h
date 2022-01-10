/*
 *   Copyright (C) 2018,2020,2021,2022 by Jonathan Naylor G4KLX
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

#if defined(USE_GPIO)

#if !defined(GPIO_H)
#define	GPIO_H

#include <string>

#include <gpiod.h>

class CGPIO {
public:
	CGPIO(unsigned int txPin, bool txInvert, unsigned int rcvPin, bool rcvInvert, unsigned int pttPin, bool pttInvert, unsigned int volumeUpPin, unsigned int volumeDownPin, bool volumeInvert);
	~CGPIO();

	bool open();

	void setTX(bool tx);
	void setRCV(bool rcv);

	bool getPTT();

	bool getVolumeUp();
	bool getVolumeDown();

	void close();

private:
	unsigned int m_txPin;
	bool         m_txInvert;
	unsigned int m_rcvPin;
	bool         m_rcvInvert;
	unsigned int m_pttPin;
	bool         m_pttInvert;
	unsigned int m_volumeUpPin;
	unsigned int m_volumeDownPin;
	bool         m_volumeInvert;
	gpiod_chip*  m_chip;
	gpiod_line*  m_tx;
	gpiod_line*  m_rcv;
	gpiod_line*  m_ptt;
	gpiod_line*  m_volumeUp;
	gpiod_line*  m_volumeDown;
};

#endif

#endif
