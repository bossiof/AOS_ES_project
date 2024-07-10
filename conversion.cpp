/*
conversion program:
opens the input wav file, creates a buffer of buffersize samples.
Then converts the samples in the buffer in ADPCM and stores them into a file for reproducion.

 */

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "adpcm.h"
#include <dirent.h>
#include "filesystem/file_access.h"
using namespace std;


//header at the beginning of every WAV file, containing various informations
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

int conversion(int buffersize, const string& filename)
{
	int samples_number =0; 	//number of total samples converted
	string internal_filename = "/sd/"+ filename + ".wav"; //file name of the song to be converted
	ifstream in(internal_filename.c_str(),ios::binary);
	if(!in) return -1; 
	// Read the header
    WavHeader header;
    in.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
	//check if the WAV file is valid
    if (std::string(header.riff, 4) != "RIFF" || std::string(header.wave, 4) != "WAVE") {
		std::cerr << "Invalid WAV file!" << std::endl;
        return 1;
    }
	samples_number= (header.fileSize/8) -4; //number of ADPCM samples computed from the encoding of the total samples of the file
	// Display header information
    std::cout << "Sample Rate: " << header.sampleRate << std::endl;
    std::cout << "Bits Per Sample: " << header.bitsPerSample << std::endl;
    std::cout << "Number of Channels: " << header.numChannels << std::endl;
	//out filestream creation
	string outname= "/sd/"+filename+".txt";
	//check if there is already a converted file
	ifstream outputcheck(outname.c_str());
	if(outputcheck.good()){
		cout<<"converted file already present"<<std::endl;

		return samples_number;
	}
	//if there is no converted file already, start conversion 
	else {
		cout<<"convertion"<<std::endl;		
		ofstream  out(outname.c_str(),ios::binary);
		vector<short> data;
		vector<unsigned char> encoded;
		int ADPCMsample; //number of ADPCM converted samples for the buffer
		std::cout << "starting conversion " << std::endl;
		while(!in.eof()){
			for(int i=0;i<buffersize;i++)
			{
				char intermediate_x[4];
				short xl;
				short xr;
				short x;
				if(!in)	break;	
				//read the samples, store them in a left and right intermediate value and then push back their average (mono)
				in.read(reinterpret_cast<char*>(&intermediate_x[0]),4);
				if(!in)	break;	
				xl = (intermediate_x[1]<<8)| intermediate_x[0];
				xr = (intermediate_x[3]<<8)| intermediate_x[2];	
				x = (xr>>1) + (xl>>1);		
				data.push_back(x);
			}
			
			
			ADPCMsample=data.size()%2==0?data.size():(data.size()-1);
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
		std::cout<<header.fileSize<<"is the number of unconverted samples "<<std::endl;
		std::cout<<samples_number<<" is the samples number "<<std::endl; 
		std::cout << "conversion completed" << std::endl;
		return samples_number;
	}
	
}

