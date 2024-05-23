#include <string>
#include <vector>
#include "player.h"
#include "reproducible_audio.h"
#include "miosix.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include "convert_2.h"
#include <cstdint>
#include <cstdlib>



using namespace std;

int main()
{	
	
	//sdcard test

	/*info = stat("/sd",&buffer) ;
	if (errno != 0)	{
		printf("sd cannot be mounted\n");
	}
	else if(info){
		printf("sd is mounted\n");
	}
	FILE* audiofile = fopen("/sd/prova.txt", "r");
	if(audiofile){
		printf("file not empty\n");
	} 
	else{
		printf("file empty, error\n");
	}*/



	int buffersize=8000;//buffer passed to the conversion function 
	int playbuffer = 8000; //buffer used for audio playback

	int audio_size;
	string filename;
	filename= "/sd/reproducible_audio.txt";

	audio_size = conversion(buffersize);

	printf("conversion is over, number of samples: %d \n", audio_size);

	//vector<unsigned char> reproducible_audio_vector; //vector used for the playback buffer

	unsigned char *reproducible_audio_array; //array used for the playback buffer
	reproducible_audio_array = (unsigned char*)malloc(playbuffer);

	ifstream audio_bin_file (filename.c_str(),ios::binary);

	printf("starting audio reproduction:\n");

	
	for(int i=0;i<audio_size;i+=playbuffer){

		//VECTOR IMPLEMENTATION

		// for(int i=0;i<(playbuffer);i++)
		// {
		// 	unsigned char x;
		// 	audio_bin_file.read(reinterpret_cast<char*>(&x),1);
		// 	if(!audio_bin_file) break;
		// 	reproducible_audio_vector.push_back(x);
		// 	//cout<<"passed\n";
		// 	//cout<<reproducible_audio_array[i]<<"\n";
		// }
			
			//ARRAY IMPLEMENTATION
			

			audio_bin_file.read(reinterpret_cast<char*>(reproducible_audio_array),playbuffer);
			if(!audio_bin_file) break;
			
			




		//cout<<"buffer filled\n";
		//if (reproducible_audio_array.back()=='\0') break;
		// ADPCMSound sound(reproducible_audio_vector.data(),playbuffer);
		// Player::instance().play(sound);
		// reproducible_audio_vector.clear();
		ADPCMSound sound(reproducible_audio_array,playbuffer);
		Player::instance().play(sound);
	}
	printf("audio reproduction is over\n");
	audio_bin_file.close();
	







}
