/***************************************************************************
 *   Copyright (C) 2024 by Terraneo Federico and Francesco Bossio          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#pragma once

#include "player.h"
#include <fstream>

 //FileSound is a subclass of the Sound class, used to reproduce the entire audio of an ADPCM encoded file 
class ADPCMFileSound : public Sound
{
public:
	ADPCMFileSound(const std::string& path);

	virtual bool fillMonoBuffer(unsigned short *buffer, int length);
    
	virtual bool fillStereoBuffer(unsigned short *buffer, int length);

	virtual void rewind();

	virtual ~ADPCMFileSound();

private:
	std::ifstream audio_bin_file;
	int file_size;
	int remaining;
	char *adpcm_buffer=nullptr;
	int adpcm_buffer_size=0;
};

