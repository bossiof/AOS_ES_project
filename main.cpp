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
#include "filesystem/file_access.h"
#include "selection.h"
#include "adpcm_file.h"


using namespace std;

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
		ADPCMFileSound file_sound(filename); //instance of the subclass Filesound with the song's file name as parameter
		Player::instance().play(file_sound);//the method play handles the reproduction
		printf("audio reproduction is over\n");
		printf("restart?\n");
	}
	return 0;
}
