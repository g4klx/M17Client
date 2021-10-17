/*
 *	Copyright (C) 2009,2015,2021 by Jonathan Naylor, G4KLX
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

#ifndef	AudioBackend_H
#define	AudioBackend_H

#include "AudioCallback.h"

class IAudioBackend {
public:
	virtual void setCallback(IAudioCallback* callback, int id = 0) = 0;
	virtual bool open() = 0;
	virtual void close() = 0;

	virtual bool isWriterBusy() const = 0;

private:
};

#endif
