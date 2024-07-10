#include <string>
#include <vector>
#include "player.h"
#include "reproducible_audio.h"
#include "miosix.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include "conversion.h"
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include "filesystem/file_access.h"
#include "selection.h"
#include "adpcm.h"


using namespace std;

class FileSound : public Sound
{
public:
	FileSound(const string& path);

	virtual bool fillMonoBuffer(unsigned short *buffer, int length);
    
	virtual bool fillStereoBuffer(unsigned short *buffer, int length);

	virtual void rewind();

	virtual ~FileSound();

private:
	ifstream audio_bin_file;
	int file_size;
	int remaining;
	char *adpcm_buffer=nullptr;
	int adpcm_buffer_size=0;
};

FileSound::FileSound(const string& path)
{
	audio_bin_file.open(path,ios::binary);
	audio_bin_file.seekg(0,ios::end);
	file_size=audio_bin_file.tellg();
	rewind();
}

bool FileSound::fillMonoBuffer(unsigned short *buffer, int length)
{
	assert(false);
}

bool FileSound::fillStereoBuffer(unsigned short *buffer, int length)
{
	if(adpcm_buffer==nullptr)
	{
		adpcm_buffer=new char[length/4];
		adpcm_buffer_size=length/4;
	}
	else assert(adpcm_buffer_size==length/4);

	int to_read=min(adpcm_buffer_size,remaining);
	audio_bin_file.read(adpcm_buffer,to_read);
	remaining-=to_read;
	int index=0;
	for(int i=0;i<to_read;i++)
	{
		unsigned short sample;
		sample=ADPCM_Decode(adpcm_buffer[i] & 0xf);
		buffer[index++]=sample;
		buffer[index++]=sample;
		sample=ADPCM_Decode(adpcm_buffer[i]>>4);
		buffer[index++]=sample;
		buffer[index++]=sample;
	}
	while(index<length) buffer[index++]=0;
	return remaining==0;
}

void FileSound::rewind()
{
	audio_bin_file.seekg(0,ios::beg);
	remaining=file_size;
}

FileSound::~FileSound()
{
	if(adpcm_buffer) delete[] adpcm_buffer;
}

int main()
{
	int buffersize=16000;//buffer passed to the conversion function 
	//int playbuffer = 28000; //buffer used for audio playback
	int audio_size; //audio size (in number of samples) used for audio playback and also for information purposes
	string convertible_filename;
	string filename;
	//the conversion takes the raw file from the microsd and converts it into an ADPCM encoded file (also on the microsd)
	for(int reproduction_counter=0;  reproduction_counter<10; reproduction_counter++ ){
		convertible_filename = selection();
		audio_size = conversion(buffersize, convertible_filename);
		filename= "/sd/"+convertible_filename+".txt";
		if(audio_size<2){
			Player::instance().trail();
			return 0;
		}

		printf("conversion is over, number of samples: %d \n", audio_size);


		//auto reproducible_audio_array = new unsigned char[playbuffer];
		//ifstream audio_bin_file (filename.c_str(),ios::binary);

		printf("starting audio reproduction:\n");
		//Player::instance().initialize(); //here i initialize the player to avoid eventual overhead during audio reproduction
		FileSound file_sound(filename);
		Player::instance().play(file_sound);
		//audio reproduction phase
		//while(audio_size>0) {
		//	int toRead=min(audio_size,playbuffer);
		//	audio_bin_file.read(reinterpret_cast<char*>(reproducible_audio_array),toRead);
		//	ADPCMSound sound(reproducible_audio_array,toRead);
		//	Player::instance().single_play(sound); //here i reproduce a single audio buffer read from the input file
		//	audio_size-=toRead;
		//}
		//Player::instance().trail();
		printf("audio reproduction is over\n");
		//delete[] reproducible_audio_array;
		printf("restart?\n");
	}
	
	return 0;

}
