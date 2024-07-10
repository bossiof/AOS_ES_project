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


using namespace std;

int main()
{	
	


	int buffersize=16000;//buffer passed to the conversion function 
	int playbuffer = 28000; //buffer used for audio playback
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


		auto reproducible_audio_array = new unsigned char[playbuffer];
		ifstream audio_bin_file (filename.c_str(),ios::binary);

		printf("starting audio reproduction:\n");
		Player::instance().initialize(); //here i initialize the player to avoid eventual overhead during audio reproduction
		//audio reproduction phase
		while(audio_size>0) {

			int toRead=min(audio_size,playbuffer);
			audio_bin_file.read(reinterpret_cast<char*>(reproducible_audio_array),toRead);
			ADPCMSound sound(reproducible_audio_array,toRead);
			Player::instance().single_play(sound); //here i reproduce a single audio buffer read from the input file
			audio_size-=toRead;
		}
		Player::instance().trail();
		printf("audio reproduction is over\n");
		delete[] reproducible_audio_array;
		printf("restart?\n");
	}
	
	return 0;

}
