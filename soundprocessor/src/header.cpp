#include "audiofile.h"
#include <iostream>
#include <fstream>
#include "convasciiint.h"
#include "writer.h"
#include "excpts.h"
#include "reader.h"

wavheader::wavheader() : average_bytes_per_sec(0), block_align(0),
						 comp_code(0), n_of_channels(0), sample_rate(0), 
						 sign_bits_per_sample(0), RIFF_chunk_size(0) {}

void wavheader::read_header(std::ifstream& wavin, const char* filename) {
	char buffer[5];
	buffer[4] = '\0';
	WavReader reader;

	wavin.read(buffer, 4);
	if(std::string(buffer).compare("RIFF"))
		throw wrong_header(filename, buffer);
	
	RIFF_chunk_size = reader.read_int_b(wavin, 4);

	wavin.read(buffer, 4);
	if (std::string(buffer).compare("WAVE"))
		throw wrong_header(filename, buffer);

	wavin.read(buffer, 4);
	if (std::string(buffer).compare("fmt "))
		throw wrong_header(filename, buffer);

	wavin.read(buffer, 4);
	if (ascii_seq_to_int_le(buffer, 4) != 16)
		throw wrong_header(filename, buffer);

	comp_code = reader.read_int_b(wavin, 2);
	n_of_channels = reader.read_int_b(wavin, 2);
	sample_rate = reader.read_int_b(wavin, 4);
	average_bytes_per_sec = reader.read_int_b(wavin, 4);
	block_align = reader.read_int_b(wavin, 2);
	sign_bits_per_sample = reader.read_int_b(wavin, 2);
}

void wavheader::genarate_header(std::ofstream& f) {
	WavWriter writer;

	f.write("RIFF", 4);
	writer.write_num_b(0, f, 4); //File size - 8
	f.write("WAVE", 4);
	f.write("fmt ", 4);

	writer.write_num_b(16, f, 4);		 //size of fmt data
	writer.write_num_b(1, f, 2);		 //compression type
	writer.write_num_b(1, f, 2);		 //number of channels
	writer.write_num_b(44100, f, 4);	 //discr
	writer.write_num_b(88200, f, 4);	 //bitrate
	writer.write_num_b(2, f, 2);		 //allign
	writer.write_num_b(16, f, 2);		 //audio depth
	f.write("data", 4);
	writer.write_num_b(200000202, f, 4); //file data size
}

unsigned int wavheader::get_sample_rate() const {
	return sample_rate;
}

wavheader::~wavheader(){}
