/*
 *   Copyright (C) 2020,2021 by Jonathan Naylor G4KLX
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

#if !defined(DEFINES_H)
#define  DEFINES_H

const unsigned int CODEC_SAMPLE_RATE = 8000U;
const unsigned int CODEC_BLOCK_SIZE  = CODEC_SAMPLE_RATE / 25U;

const unsigned char MODE_M17   = 7U;

const unsigned char TAG_HEADER = 0x00U;
const unsigned char TAG_DATA1  = 0x01U;
const unsigned char TAG_DATA2  = 0x02U;
const unsigned char TAG_LOST   = 0x03U;
const unsigned char TAG_EOT    = 0x04U;

enum RPT_RF_STATE {
	RS_RF_LISTENING,
	RS_RF_LATE_ENTRY,
	RS_RF_AUDIO,
	RS_RF_DATA,
	RS_RF_INVALID
};

#endif
