#include "weld_arch.hpp"

std::vector<packet_draf> chunk_custom_buffors(const std::vector<unsigned char> &stat_buff,
                                                               size_t all_size,
                                                               short int num_of_dst) {

	assert(num_of_dst > 0  && "Can not part to zero parts");
	assert(stat_buff.size() > 0 && "Can not part zero size vector");
	if(num_of_dst > all_size) {
		//std::cerr << "WARNING, dst number is greater than custom data size : decreasing parts to max ["
		//		  << all_size << "] parts\n";
		num_of_dst = all_size;
	}

	std::vector<packet_draf> vec_parts;

	size_t actual_pos = 0;
	size_t main_len = all_size;
	for (short int i = 0;i < num_of_dst; ++i) {
		double end_dist = (i+1)*(main_len/static_cast<double>(num_of_dst));

		auto begin_pos = stat_buff.begin() + actual_pos;
		actual_pos = std::ceil(end_dist);
		auto end_pos = (stat_buff.begin() + all_size +1) - ((actual_pos >= main_len) ? 0 : (main_len - actual_pos));
		packet_draf loc {i,  std::vector<unsigned char>(begin_pos, end_pos)};
		vec_parts.emplace_back(std::move(loc));
	}
	return vec_parts;
}

std::vector<packet_draf> generate_random_packets(size_t packets_num, short int num_of_dst) {

	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<short int> rn_dst(1, num_of_dst);
	std::uniform_int_distribution<> rn_size(64,64);

	std::vector<packet_draf> l_packets;
	l_packets.reserve(packets_num);
	for (size_t i=0; i < packets_num; ++i) {
		// starting from A , B ...
		auto rand_dst = rn_dst(gen);
		auto rand_data = generate_random_uchar_vector(rn_size(gen));
		packet_draf packet {rand_dst, rand_data};
		l_packets.emplace_back(packet);
	}
	return l_packets;
}

void weld_manager_continous::eat(std::vector<packet_draf> &&packets) {

	for(auto &pac : packets) {

		auto &dst_buff = all_data[pac.m_dst];
		if(!dst_buff.empty())
			dst_buff.push_back(' ');
		dst_buff.insert(all_data[pac.m_dst].end(), pac.m_data.begin(), pac.m_data.end());
	}
}

void weld_manager_continous::encrypt_all_buffers(std::array<unsigned char, crypto_secretbox_NONCEBYTES> &nonce,
                                                 std::array<unsigned char, crypto_secretbox_KEYBYTES> &key) {

	for(auto &data : all_data) {
		data.second = secretbox_easy_encrypt(data.second, nonce, key);
	}
}

void weld_manager_continous::decrypt_all_buffers(std::array<unsigned char, crypto_secretbox_NONCEBYTES> &nonce,
                                                 std::array<unsigned char, crypto_secretbox_KEYBYTES> &key) {

	for(auto &data : all_data) {
		data.second = secretbox_easy_decrypt(data.second, nonce, key);
	}
}


void weld_manager_continous::print_status() {

	size_t num_of_buffers = 0;
	size_t size_all = 0;
	size_t max_buffer_size = 0;
	size_t min_buffer_size = std::numeric_limits<size_t>::max();
	for(auto record : all_data) {
		if ( record.second.size() == 0 ) continue;
		size_t size_of_record = record.second.size();
		num_of_buffers++;
		size_all += size_of_record;
		if (size_of_record < min_buffer_size) {
			min_buffer_size = size_of_record;
		}
		if (size_of_record > max_buffer_size) {
			max_buffer_size = size_of_record;
		}
	}
	size_t avr_buffer_size = size_all/num_of_buffers;
		std::cout << "num of buffers: " << num_of_buffers
				  << ", size of all buffers: " << size_all
				  << ", min size: " << min_buffer_size
				  << ", max size: " << max_buffer_size
				  << ", average size: " << avr_buffer_size << '\n';
}

void weld_manager_continous::print_packets() {

	for(auto record : all_data) {
		std::cout << "dst: " << record.first
				  << ", size: " << record.second.size()
				  << " ,data: [" << uchar_vector_tostring(record.second,false)<< "]\n";
	}
}
