/*
conversion program:
we open the input wav file, we create a buffer of 10000 samples.
we then convert them in ADPCM and store them into a vector for reproduction and a file.

 */

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include "adpcm.h"
#include "filesystem/file_access.h"
#include <dirent.h>
#include"conversion.h"
using namespace std;



#pragma pack(push, 1)
struct WavHeader 
{
    char riff[4];        // "RIFF"
    uint32_t fileSize;   // File size - 8 bytes
    char wave[4];        // "WAVE"
    char fmt[4];         // "fmt "
    uint32_t fmtLength;  // Length of format data
    uint16_t audioFormat;// Audio format (1 for PCM)
    uint16_t numChannels;// Number of channels
    uint32_t sampleRate; // Sample rate
    uint32_t byteRate;   // Byte rate
    uint16_t blockAlign; // Block align
    uint16_t bitsPerSample;// Bits per sample
    char data[4];        // "data"
    uint32_t dataSize;   // Size of the data section
};
#pragma pack(pop)

void fail(const char *err)
{
    puts(err);
    exit(1);
}

string selection()
{
	//file listing
	cout<<"available files:"<<std::endl<<"\n";
	DIR *d=opendir("/sd/");
	if(d==NULL) fail("Can't open directory");
	struct dirent *de;
	int songindex = 0;
	vector<string> songnames;
	//vector<string> converted_files;
	string extensionless_songname;
	int song_selector=0;
	while((de=readdir(d)))
    {

		songnames.push_back(de->d_name);
		if(songnames.back().substr(songnames.back().find_last_of(".") + 1) == "wav"){
			songindex++;
			cout<<songindex<<")";
			puts(de->d_name);
			cout<<std::endl;
		}
		/*else if(songnames.back().substr(songnames.back().find_last_of(".") + 1) == "txt"){
			converted_files.push_back(songnames.back());
			songnames.pop_back();
		}*/
		else {
			songnames.pop_back();
		}



    }
    closedir(d);

	//file selection
	printf("select your song\n");
	do {
	scanf("%d", &song_selector);
	if ((song_selector<1) || (song_selector>songindex)) {
		cout<< "wrong song number, retry"<<std::endl;
	}
	}while ((song_selector<1) || (song_selector>songindex));
	
	printf("you choose song number %d\n",song_selector);
	extensionless_songname = songnames[song_selector-1]; 
	extensionless_songname.erase(extensionless_songname.find_last_of("."));
	string filename;
	filename= extensionless_songname;
	return filename;
}

