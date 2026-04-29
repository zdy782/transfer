#include "global_buffers.hpp"
#include <iostream>

std::vector<unsigned char> generate_randombyte_buffer(size_t size) {

	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> dis(0, std::numeric_limits<unsigned char>::max());

	std::vector<unsigned char> vector_data;
	vector_data.reserve(size);
	std::cout << "Generating big buffer: " << size << "bytes\n";
	size_t step = size/100; // 1% from 100% step
	for(size_t i=0, j=0; i<size; ++i) {
		if(i%step == 0) {
			std::cout << "\r";
			std::cout << "[" << j << "%] done";
			std::cout << std::flush;
			j+=1;
		}

		vector_data.push_back(dis(gen));
	}
	std::cout << '\n';
	return vector_data;
}
