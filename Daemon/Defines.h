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

const unsigned int SOUNDCARD_SAMPLE_RATE = 48000U;
const unsigned int SOUNDCARD_BLOCK_SIZE  = CODEC_BLOCK_SIZE * (SOUNDCARD_SAMPLE_RATE / CODEC_SAMPLE_RATE);

const unsigned char MODE_IDLE    = 0U;
const unsigned char MODE_DSTAR   = 1U;
const unsigned char MODE_DMR     = 2U;
const unsigned char MODE_YSF     = 3U;
const unsigned char MODE_P25     = 4U;
const unsigned char MODE_NXDN    = 5U;
const unsigned char MODE_POCSAG  = 6U;
const unsigned char MODE_M17     = 7U;

const unsigned char MODE_FM      = 10U;

const unsigned char MODE_CW      = 98U;
const unsigned char MODE_LOCKOUT = 99U;
const unsigned char MODE_ERROR   = 100U;
const unsigned char MODE_QUIT    = 110U;

const unsigned char TAG_HEADER = 0x00U;
const unsigned char TAG_DATA1  = 0x01U;
const unsigned char TAG_DATA2  = 0x02U;
const unsigned char TAG_LOST   = 0x03U;
const unsigned char TAG_EOT    = 0x04U;

enum RPT_RF_STATE {
	RS_RF_LISTENING,
	RS_RF_LATE_ENTRY,
	RS_RF_AUDIO,
	RS_RF_AUDIO_DATA,
	RS_RF_DATA,
	RS_RF_INVALID
};

enum HW_TYPE {
	HWT_MMDVM,
	HWT_DVMEGA,
	HWT_MMDVM_ZUMSPOT,
	HWT_MMDVM_HS_HAT,
	HWT_MMDVM_HS_DUAL_HAT,
	HWT_NANO_HOTSPOT,
	HWT_NANO_DV,
	HWT_D2RG_MMDVM_HS,
	HWT_MMDVM_HS,
	HWT_OPENGD77_HS,
	HWT_SKYBRIDGE,
	HWT_UNKNOWN
};

#endif
