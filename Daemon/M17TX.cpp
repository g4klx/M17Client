/*
 *	Copyright (C) 2020,2021 Jonathan Naylor, G4KLX
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; version 2 of the License.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 */

#include "M17TX.h"
#include "M17Convolution.h"
#include "M17Utils.h"
#include "M17CRC.h"
#include "Golay24128.h"
#include "Utils.h"
#include "Log.h"

#include <cstdio>
#include <cassert>
#include <cstring>
#include <ctime>

const unsigned int INTERLEAVER[] = {
	0U, 137U, 90U, 227U, 180U, 317U, 270U, 39U, 360U, 129U, 82U, 219U, 172U, 309U, 262U, 31U, 352U, 121U, 74U, 211U, 164U,
	301U, 254U, 23U, 344U, 113U, 66U, 203U, 156U, 293U, 246U, 15U, 336U, 105U, 58U, 195U, 148U, 285U, 238U, 7U, 328U, 97U,
	50U, 187U, 140U, 277U, 230U, 367U, 320U, 89U, 42U, 179U, 132U, 269U, 222U, 359U, 312U, 81U, 34U, 171U, 124U, 261U, 214U,
	351U, 304U, 73U, 26U, 163U, 116U, 253U, 206U, 343U, 296U, 65U, 18U, 155U, 108U, 245U, 198U, 335U, 288U, 57U, 10U, 147U,
	100U, 237U, 190U, 327U, 280U, 49U, 2U, 139U, 92U, 229U, 182U, 319U, 272U, 41U, 362U, 131U, 84U, 221U, 174U, 311U, 264U,
	33U, 354U, 123U, 76U, 213U, 166U, 303U, 256U, 25U, 346U, 115U, 68U, 205U, 158U, 295U, 248U, 17U, 338U, 107U, 60U, 197U,
	150U, 287U, 240U, 9U, 330U, 99U, 52U, 189U, 142U, 279U, 232U, 1U, 322U, 91U, 44U, 181U, 134U, 271U, 224U, 361U, 314U, 83U,
	36U, 173U, 126U, 263U, 216U, 353U, 306U, 75U, 28U, 165U, 118U, 255U, 208U, 345U, 298U, 67U, 20U, 157U, 110U, 247U, 200U,
	337U, 290U, 59U, 12U, 149U, 102U, 239U, 192U, 329U, 282U, 51U, 4U, 141U, 94U, 231U, 184U, 321U, 274U, 43U, 364U, 133U, 86U,
	223U, 176U, 313U, 266U, 35U, 356U, 125U, 78U, 215U, 168U, 305U, 258U, 27U, 348U, 117U, 70U, 207U, 160U, 297U, 250U, 19U,
	340U, 109U, 62U, 199U, 152U, 289U, 242U, 11U, 332U, 101U, 54U, 191U, 144U, 281U, 234U, 3U, 324U, 93U, 46U, 183U, 136U, 273U,
	226U, 363U, 316U, 85U, 38U, 175U, 128U, 265U, 218U, 355U, 308U, 77U, 30U, 167U, 120U, 257U, 210U, 347U, 300U, 69U, 22U,
	159U, 112U, 249U, 202U, 339U, 292U, 61U, 14U, 151U, 104U, 241U, 194U, 331U, 284U, 53U, 6U, 143U, 96U, 233U, 186U, 323U,
	276U, 45U, 366U, 135U, 88U, 225U, 178U, 315U, 268U, 37U, 358U, 127U, 80U, 217U, 170U, 307U, 260U, 29U, 350U, 119U, 72U,
	209U, 162U, 299U, 252U, 21U, 342U, 111U, 64U, 201U, 154U, 291U, 244U, 13U, 334U, 103U, 56U, 193U, 146U, 283U, 236U, 5U,
	326U, 95U, 48U, 185U, 138U, 275U, 228U, 365U, 318U, 87U, 40U, 177U, 130U, 267U, 220U, 357U, 310U, 79U, 32U, 169U, 122U,
	259U, 212U, 349U, 302U, 71U, 24U, 161U, 114U, 251U, 204U, 341U, 294U, 63U, 16U, 153U, 106U, 243U, 196U, 333U, 286U, 55U,
	8U, 145U, 98U, 235U, 188U, 325U, 278U, 47U};

const unsigned char SCRAMBLER[] = {
	0x00U, 0x00U, 0xD6U, 0xB5U, 0xE2U, 0x30U, 0x82U, 0xFFU, 0x84U, 0x62U, 0xBAU, 0x4EU, 0x96U, 0x90U, 0xD8U, 0x98U, 0xDDU,
	0x5DU, 0x0CU, 0xC8U, 0x52U, 0x43U, 0x91U, 0x1DU, 0xF8U, 0x6EU, 0x68U, 0x2FU, 0x35U, 0xDAU, 0x14U, 0xEAU, 0xCDU, 0x76U,
	0x19U, 0x8DU, 0xD5U, 0x80U, 0xD1U, 0x33U, 0x87U, 0x13U, 0x57U, 0x18U, 0x2DU, 0x29U, 0x78U, 0xC3U};

const unsigned char BIT_MASK_TABLE[] = { 0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U };

#define WRITE_BIT(p,i,b) p[(i)>>3] = (b) ? (p[(i)>>3] | BIT_MASK_TABLE[(i)&7]) : (p[(i)>>3] & ~BIT_MASK_TABLE[(i)&7])
#define READ_BIT(p,i)    (p[(i)>>3] & BIT_MASK_TABLE[(i)&7])

CM17TX::CM17TX(const std::string& callsign, unsigned int can, const std::string& text) :
m_transmit(false),
m_queue(5000U, "M17 TX"),
m_frames(0U),
m_emptyLSF(),
m_textLSF(),
m_gpsLSF(),
m_lsfN(0U)
{
	m_emptyLSF.setSource(callsign);
	m_textLSF.setSource(callsign);
	m_gpsLSF.setSource(callsign);
	
	m_emptyLSF.setCAN(can);
	m_textLSF.setCAN(can);
	m_gpsLSF.setCAN(can);
}

CM17TX::~CM17TX()
{
}

unsigned int CM17TX::read(unsigned char* data)
{
	assert(data != NULL);

	if (m_queue.isEmpty())
		return 0U;

	unsigned char len = 0U;
	m_queue.getData(&len, 1U);

	m_queue.getData(data, len);

	return len;
}

void CM17TX::write(const unsigned char* audio, bool end)
{
	assert(audio != NULL);

	if (!m_transmit) {
		m_frames = 0U;
		m_lsfN   = 0U;

		// Create a dummy start message
		unsigned char start[M17_FRAME_LENGTH_BYTES + 2U];

		start[0U] = TAG_HEADER;
		start[1U] = 0x00U;

		// Generate the sync
		addLinkSetupSync(start + 2U);

		unsigned char setup[M17_LSF_LENGTH_BYTES];
		m_emptyLSF.getLinkSetup(setup);

		// Add the convolution FEC
		CM17Convolution conv;
		conv.encodeLinkSetup(setup, start + 2U + M17_SYNC_LENGTH_BYTES);

		unsigned char temp[M17_FRAME_LENGTH_BYTES];
		interleaver(start + 2U, temp);
		decorrelator(temp, start + 2U);

		writeQueue(start);
		
		m_transmit = true;
	}

	if (m_transmit) {
		unsigned char data[M17_FRAME_LENGTH_BYTES + 2U];

		data[0U] = TAG_DATA1;
		data[1U] = 0x00U;

		// Generate the sync
		addStreamSync(data + 2U);

		// Add the fragment LICH
		unsigned char lich[M17_LICH_FRAGMENT_LENGTH_BYTES];
		m_emptyLSF.getFragment(lich, m_lsfN);

		// Add the fragment number
		lich[5U] = (m_lsfN & 0x07U) << 5;

		unsigned int frag1, frag2, frag3, frag4;
		CM17Utils::splitFragmentLICH(lich, frag1, frag2, frag3, frag4);

		// Add Golay to the LICH fragment here
		unsigned int lich1 = CGolay24128::encode24128(frag1);
		unsigned int lich2 = CGolay24128::encode24128(frag2);
		unsigned int lich3 = CGolay24128::encode24128(frag3);
		unsigned int lich4 = CGolay24128::encode24128(frag4);

		CM17Utils::combineFragmentLICHFEC(lich1, lich2, lich3, lich4, data + 2U + M17_SYNC_LENGTH_BYTES);

		uint16_t fn = m_frames;
		if (end)
			fn |= 0x8000U;

		unsigned char payload[M17_FN_LENGTH_BYTES + M17_PAYLOAD_LENGTH_BYTES];

		// Add the FN
		payload[0U] = (fn >> 8) & 0xFFU;
		payload[1U] = (fn >> 0) & 0xFFU;

		// Add the data/audio
		::memcpy(payload + M17_FN_LENGTH_BYTES, audio, M17_PAYLOAD_LENGTH_BYTES);

		// Add the Convolution FEC
		CM17Convolution conv;
		conv.encodeData(payload, data + 2U + M17_SYNC_LENGTH_BYTES + M17_LICH_FRAGMENT_FEC_LENGTH_BYTES);

		unsigned char temp[M17_FRAME_LENGTH_BYTES];
		interleaver(data + 2U, temp);
		decorrelator(temp, data + 2U);

		writeQueue(data);

		m_lsfN++;
		if (m_lsfN >= 6U)
			m_lsfN = 0U;

		m_frames++;

		if (end)
			m_transmit = false;
	}
}

void CM17TX::writeQueue(const unsigned char *data)
{
	assert(data != NULL);

	unsigned char len = M17_FRAME_LENGTH_BYTES + 2U;

	unsigned int space = m_queue.freeSpace();
	if (space < (len + 1U)) {
		LogError("M17, overflow in the M17 RF queue");
		return;
	}

	m_queue.addData(&len, 1U);

	m_queue.addData(data, len);
}

void CM17TX::interleaver(const unsigned char* in, unsigned char* out) const
{
	assert(in != NULL);
	assert(out != NULL);

	for (unsigned int i = 0U; i < (M17_FRAME_LENGTH_BITS - M17_SYNC_LENGTH_BITS); i++) {
		unsigned int n1 = i + M17_SYNC_LENGTH_BITS;
		bool b = READ_BIT(in, n1) != 0U;
		unsigned int n2 = INTERLEAVER[i] + M17_SYNC_LENGTH_BITS;
		WRITE_BIT(out, n2, b);
	}
}

void CM17TX::decorrelator(const unsigned char* in, unsigned char* out) const
{
	assert(in != NULL);
	assert(out != NULL);

	for (unsigned int i = M17_SYNC_LENGTH_BYTES; i < M17_FRAME_LENGTH_BYTES; i++)
		out[i] = in[i] ^ SCRAMBLER[i];
}

void CM17TX::addLinkSetupSync(unsigned char* data)
{
	assert(data != NULL);

	::memcpy(data, M17_LINK_SETUP_SYNC_BYTES, M17_SYNC_LENGTH_BYTES);
}

void CM17TX::addStreamSync(unsigned char* data)
{
	assert(data != NULL);

	::memcpy(data, M17_STREAM_SYNC_BYTES, M17_SYNC_LENGTH_BYTES);
}

