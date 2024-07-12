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

#include "adpcm_file.h"
#include "adpcm.h"
#include <cassert>

using namespace std;

 //constructor of the subclass, it opens the file, checks the filesize and rewinds the file to the beginning
ADPCMFileSound::ADPCMFileSound(const string& path)
{
	audio_bin_file.open(path,ios::binary);
	audio_bin_file.seekg(0,ios::end);
	file_size=audio_bin_file.tellg();
	rewind();
}
//method to fill the mono double buffer (to be implemented)
bool ADPCMFileSound::fillMonoBuffer(unsigned short *buffer, int length)
{
	assert(false);
}
//method used to fill the stereo double buffer to be reproduced
bool ADPCMFileSound::fillStereoBuffer(unsigned short *buffer, int length)
{
	//if there isn't already an ADPCM buffer, create one with the size of the total ADPCM decoded samples
	if(adpcm_buffer==nullptr)
	{
		adpcm_buffer=new char[length/4];
		adpcm_buffer_size=length/4;
	}
	else assert(adpcm_buffer_size==length/4); //if there is already an ADPCM buffer and it is not the right size, stop the program

	int to_read=min(adpcm_buffer_size,remaining); //the number of samples to read is the minimum between buffer size and the remaining number of samples
	audio_bin_file.read(adpcm_buffer,to_read); //read the binary file with the right number of samples and save them in the ADPCM buffer
	remaining-=to_read;//the remaining number of samples is decremented with the number of samples read
	int index=0;//index of the samples in the internal buffer used for ADPCM conversion
	for(int i=0;i<to_read;i++)
	{
		//for each one of the element we have in the ADPCM buffer, apply ADPCM decoding and fill 4 samples of the reproducible buffer
		unsigned short sample;
		sample=ADPCM_Decode(adpcm_buffer[i] & 0xf);
		buffer[index++]=sample;
		buffer[index++]=sample;
		sample=ADPCM_Decode(adpcm_buffer[i]>>4);
		buffer[index++]=sample;
		buffer[index++]=sample;
	}
	while(index<length) buffer[index++]=0; //fill all of the remaining samples in the reproducible buffer with zeros
	return remaining==0; //return 1 if there are remaining samples, 0 otherwise
}
//method used to go back to the first position of the audio file, "rewinding" the song to the beginning
void ADPCMFileSound::rewind()
{
	audio_bin_file.seekg(0,ios::beg);
	remaining=file_size;
}
//class destructor
ADPCMFileSound::~ADPCMFileSound()
{
	if(adpcm_buffer) delete[] adpcm_buffer;
}
