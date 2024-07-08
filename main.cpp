#include <string>
#include <vector>
#include "player.h"
#include "reproducible_audio.h"
#include "miosix.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include "conversion.h"
#include <cstdint>
#include <cstdlib>
#include "filesystem/file_access.h"


using namespace std;

int main()
{	
	


	int buffersize=16000;//buffer passed to the conversion function 
	int playbuffer = 16384; //buffer used for audio playback
	int audio_size; //audio size (in number of samples) used for audio playback and also for information purposes
	string filename;
	filename= "/sd/reproducible_audio.txt";
	//the conversion takes the raw file from the microsd and converts it into an ADPCM encoded file (also on the microsd)
	for(int reproduction_counter=0;  reproduction_counter<10; reproduction_counter++ ){
	audio_size = conversion(buffersize);

	if(audio_size<2){
		Player::instance().trail();
		return 0;
	}

	printf("conversion is over, number of samples: %d \n", audio_size);


	unsigned char *reproducible_audio_array; //array used for the playback buffer
	reproducible_audio_array = (unsigned char*)malloc(playbuffer);
	unsigned char *last_samples_array;
	int multiplier = (((audio_size-(audio_size/playbuffer)*playbuffer)-1)/4)*4;
	last_samples_array = (unsigned char*)malloc(multiplier);
	ADPCMSound sound(reproducible_audio_array,playbuffer);
	ADPCMSound last_sound(last_samples_array,multiplier);

	ifstream audio_bin_file (filename.c_str(),ios::binary);

	printf("starting audio reproduction:\n");
	Player::instance().initialize(); //here i initialize the player to avoid eventual overhead during audio reproduction
	//audio reproduction phase
	for(int i=0;i<audio_size;i+=playbuffer){
		if(i>=audio_size-playbuffer){
		audio_bin_file.read(reinterpret_cast<char*>(last_sound.soundData),(multiplier));	
		Player::instance().single_play(last_sound);
		break;
		}
		audio_bin_file.read(reinterpret_cast<char*>(sound.soundData),playbuffer);

		
		//if(!audio_bin_file) {
		//	Player::instance().single_play(sound);	
			//Player::instance().trail();
			//printf("audio reproduction is over\n");
			//audio_bin_file.close();
		//	break;
		//}	
			
		Player::instance().single_play(sound);
			
	}
	Player::instance().trail();
	printf("audio reproduction is over\n");
	free(reproducible_audio_array);
	free(last_samples_array);
	audio_bin_file.close();
	printf("restart?\n");
	}
	
	return 0;

}
