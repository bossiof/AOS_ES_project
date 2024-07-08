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

int conversion(int buffersize)
{
	//file listing
	cout<<"available files:"<<std::endl<<"\n";
	DIR *d=opendir("/sd/");
	if(d==NULL) fail("Can't open directory");
	struct dirent *de;
	int songindex = 0;
	vector<string> songnames;
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
    
	//ifstream creation
	string filename;
	filename="/sd/"+ songnames[song_selector-1];
	ifstream in(filename.c_str(),ios::binary);
	if(!in) return -1;

	// Read the header
    WavHeader header;
    in.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
    if (std::string(header.riff, 4) != "RIFF" || std::string(header.wave, 4) != "WAVE") {
        std::cerr << "Invalid WAV file!" << std::endl;
        return 1;
    }

	// Display header information
    std::cout << "Sample Rate: " << header.sampleRate << std::endl;
    std::cout << "Bits Per Sample: " << header.bitsPerSample << std::endl;
    std::cout << "Number of Channels: " << header.numChannels << std::endl;
	//out filestream creation
	string outname= "/sd/reproducible_audio.txt";
	ofstream  out(outname.c_str(),ios::binary);
	
	
	vector<short> data;
	vector<unsigned char> encoded;
	int samples_number =0; 	//number of total samples converted
	int ADPCMsample; //number of ADPCM converted samples for the buffer
	std::cout << "starting conversion " << std::endl;
	while(!in.eof()){
		for(int i=0;i<buffersize;i++)
		{
			char intermediate_x[4];
			short xl;
			short xr;
			short x;
			if(!in) {
				break;
				//return samples_number;	
			}
			in.read(reinterpret_cast<char*>(&intermediate_x[0]),4);
			if(!in) {
				break;
				//return samples_number;	
			}
			xl = (intermediate_x[1]<<8)| intermediate_x[0];
			xr = (intermediate_x[3]<<8)| intermediate_x[2];	
			x = (xr>>1) + (xl>>1);		
			data.push_back(x);
		}
		
		
		ADPCMsample=data.size()%2==0?data.size():(data.size()-1);
		samples_number += ADPCMsample >> 1;
		//ADPCM encoding 
		for(int i=0;i<ADPCMsample;i+=2)
		{
			unsigned char in=ADPCM_Encode(data.at(i)) & 0xf;
			in|=ADPCM_Encode(data.at(i+1))<<4;
			encoded.push_back(in);
		}
		//cout<<"block up to "<<samples_number<<" written\n ";


		//file generation
		
		out.write(reinterpret_cast<char*>(&encoded[0]),encoded.size());


		//variables reset:
		data.clear();
		encoded.clear();
		
	}
	in.close();
	out.close();
	std::cout << "conversion completed" << std::endl;
	return samples_number;
}

