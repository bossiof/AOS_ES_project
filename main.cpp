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
//FileSound is a subclass of the Sound class, used to reproduce the entire audio of an ADPCM encoded file 
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
//constructor of the subclass, it opens the file, checks the filesize and rewinds the file to the beginning
FileSound::FileSound(const string& path)
{
	audio_bin_file.open(path,ios::binary);
	audio_bin_file.seekg(0,ios::end);
	file_size=audio_bin_file.tellg();
	rewind();
}
//method to fill the mono double buffer (to be implemented)
bool FileSound::fillMonoBuffer(unsigned short *buffer, int length)
{
	assert(false);
}
//method used to fill the stereo double buffer to be reproduced
bool FileSound::fillStereoBuffer(unsigned short *buffer, int length)
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
void FileSound::rewind()
{
	audio_bin_file.seekg(0,ios::beg);
	remaining=file_size;
}
//class destructor
FileSound::~FileSound()
{
	if(adpcm_buffer) delete[] adpcm_buffer;
}

int main()
{
	int buffersize=16000;//passed to the conversion function, it determines how many samples are converted to ADPCM per each iteration
	int audio_size; //audio size (in number of samples) used for audio playback and also for information purposes
	string convertible_filename; //name of the file related to the song selected (to be converted or played)
	string filename; //name of the converted file to be reproduced

	//this is the main cycle used for song selection,conversion and reproduction
	for(int reproduction_counter=0;  reproduction_counter<20; reproduction_counter++ ){
		convertible_filename = selection(); //lets you select a song from the ones available from terminal and returns the song's name
		//conversion takes the wav file from the microsd and converts it into an ADPCM encoded file (also on the microsd)
		//if the encoded file is already present, it doesn't convert it again
		audio_size = conversion(buffersize, convertible_filename);
		filename= "/sd/"+convertible_filename+".txt";//the name of the reproducible ADPCM file is obtained from the song name
		if(audio_size<2){
			Player::instance().trail(); //if the audio file is empty, play a trail of blank audio and stop
			return 0;
		}
		printf("conversion is over, number of samples: %d \n", audio_size);
		printf("starting audio reproduction:\n");

		FileSound file_sound(filename); //instance of the subclass Filesound with the song's file name as parameter
		Player::instance().play(file_sound);//the method play handles the reproduction
		printf("audio reproduction is over\n");
		printf("restart?\n");
	}
	
	return 0;

}
