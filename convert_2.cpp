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

using namespace std;


int conversion(int buffersize)
{
	//ifstream creation
	string filename;
	filename="/sd/fun_piano.raw";
	ifstream in(filename.c_str(),ios::binary);
	if(!in) return -1;
	//out filestream creation
	string outname= "/sd/reproducible_audio.txt";
	ofstream  out(outname.c_str(),ios::binary);
	
	
	vector<short> data;
	vector<unsigned char> encoded;
	int samples_number =0; 	//number of total samples converted
	int ADPCMsample; //number of ADPCM converted samples for the buffer
	while(!in.eof()){
		for(int i=0;i<buffersize;i++)
		{
			short x;
			in.read(reinterpret_cast<char*>(&x),2);
			if(!in) return samples_number;
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
		//std::ios_base & hex( std::ios_base& out );
		out.write(reinterpret_cast<char*>(&encoded[0]),encoded.size());


		//variables reset:
		data.clear();
		encoded.clear();
		
	}
	in.close();
	out.close();
	return samples_number;
}


