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



using namespace std;

int main()
{	
	



	int buffersize=8000;//buffer passed to the conversion function 
	int playbuffer = 8192; //buffer used for audio playback

	int audio_size; //audio size (in number of samples) used for audio playback and also for information purposes
	string filename;
	filename= "/sd/reproducible_audio.txt";
	//the conversion takes the raw file from the microsd and converts it into an ADPCM encoded file (also on the microsd)
	audio_size = conversion(buffersize);

	printf("conversion is over, number of samples: %d \n", audio_size);


	unsigned char *reproducible_audio_array; //array used for the playback buffer
	reproducible_audio_array = (unsigned char*)malloc(playbuffer);

	ifstream audio_bin_file (filename.c_str(),ios::binary);

	printf("starting audio reproduction:\n");
	Player::instance().initialize(); //here i initialize the player to avoid eventual overhead during audio reproduction


	//audio reproduction phase
	for(int i=0;i<audio_size;i+=playbuffer){
		audio_bin_file.read(reinterpret_cast<char*>(reproducible_audio_array),playbuffer);

		ADPCMSound sound(reproducible_audio_array,playbuffer);
		Player::instance().single_play(sound);

		if(!audio_bin_file) {
			Player::instance().trail();
			printf("audio reproduction is over\n");
			audio_bin_file.close();
			return 0; 
		}	
			
	}
	printf("audio reproduction is over\n");
	audio_bin_file.close();
	
	return 0;

}
