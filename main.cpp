
#include "player.h"
#include "reproducible_audio.h"
#include "miosix.h"
#include <cstdio>
#include <iostream>
#include <sys/stat.h>

using namespace std;
int info = 0;
struct stat buffer;

int main()
{	
	info = stat("/sd",&buffer) ;
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
	}
	ADPCMSound sound(reproducible_audio_bin,reproducible_audio_bin_len);
	Player::instance().play(sound);

}
