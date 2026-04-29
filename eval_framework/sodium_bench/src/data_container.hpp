#ifndef DATA_CONTAINER_HPP
#define DATA_CONTAINER_HPP

#include "global_buffer_variant.hpp"
#include "auth_encryption.hpp"

enum class data_type { generate, variant, variant_chunk8k };

constexpr size_t Max_Threads = 1024;
struct data_container {
	static std::array<std::atomic<bool>,Max_Threads> if_finish;

	cryptobox_keypair alice_keys;
	cryptobox_keypair bob_keys;

	std::vector<char> msg;

	// for chunks version
	std::vector<std::vector<std::vector<char>>> splitted_msg_chunks;
	std::vector<std::vector<std::vector<char>>> chunk_cipher;
	std::vector<std::vector<std::vector<char>>> chunk_result;


	std::vector<std::vector<char>> splitted_msg;
	std::vector<std::vector<char>> cipher;
	std::vector<std::vector<char>> check_result;

	std::array<unsigned char, crypto_box_NONCEBYTES> nonce;

	// one-time authentication poly1305 data
	std::vector<std::array<unsigned char,crypto_onetimeauth_BYTES>> onetime_auth_out;
	std::array<unsigned char,crypto_onetimeauth_KEYBYTES> onetime_auth_key;

	static data_container& get_m(size_t threads, size_t msg_length, data_type dat = data_type::generate) {
		std::lock_guard<std::mutex> lock(mtx);
		if(m_m == nullptr) {
			if (sodium_init() == -1) {
				throw std::runtime_error("Fail to init sodium");
			}
			m_m = std::unique_ptr<data_container>(new data_container(threads, msg_length, dat));
		}
		return *m_m;
	}
	volatile void clear_container() {
		std::lock_guard<std::mutex> lock(mtx_clean);
		if (m_m != nullptr) {
			splitted_msg.clear();
			cipher.clear();
			check_result.clear();
			m_data_filled = false;

			m_m.reset(nullptr);
		}
	}
	std::string get_base_msg() {
		return char_vector_tostring(thread_safe::concentrate_vector(splitted_msg),false);
	}
	std::string get_result_msg() {
		return char_vector_tostring(thread_safe::concentrate_vector(check_result),false);
	}
	void setup_data(data_type dat) {
		std::lock_guard<std::mutex> lock(mtx_fill_data);
		if(!m_data_filled) {
			switch(dat) {
				case data_type::generate  : fill_data_generate(); break;
				case data_type::variant   : fill_data_variant();  break;
			case data_type::variant_chunk8k : { size_t sis = 64; fill_data_variant_chunks(sis); break; }
				default:
					std::cerr << "data container: data_type not known\n";
			}
			m_data_filled = true;
		}
	}

private:
	data_container(data_container const&) = delete;
	void operator=(data_container const&) = delete;

	data_container(size_t threads, size_t msg_length, data_type dat) {
		if(threads > Max_Threads) {
			throw std::invalid_argument(std::string("Set up too many threads. threads [" + std::to_string(threads)
													+ "], Max threads [" + std::to_string(Max_Threads) + "]"));
		}
		for(size_t i = 0; i < Max_Threads; ++i) {
			// not run threds if is not necessary
			i < threads && i < msg_length  ? if_finish.at(i) = false
											: if_finish.at(i) = true;
		}

		base_msg_len = msg_length;
		threads_num = threads;

		alice_keys = generate_kyepair();
		bob_keys = generate_kyepair();
		nonce = generate_random_array<unsigned char, crypto_box_NONCEBYTES>();

		cipher.resize(threads);
		check_result.resize(threads);

		splitted_msg_chunks.resize(threads);
		chunk_cipher.resize(threads);
		chunk_result.resize(threads);

		// refactor proposal
		// It would be better be split different usages for example to child classes.
		onetime_auth_key = generate_random_array<unsigned char,crypto_onetimeauth_KEYBYTES>();
		onetime_auth_out.resize(threads);
	}

	void chunk_splitted_msg(size_t chunk_size){
		//splitted_msg_8kchunks
		{ size_t thread_index = 0;
		for(auto &thread_part: splitted_msg) {
			splitted_msg_chunks.at(thread_index) = chunk_char_vector(thread_part,chunk_size);
			chunk_cipher.at(thread_index).resize(splitted_msg_chunks.at(thread_index).size());
			chunk_result.at(thread_index).resize(splitted_msg_chunks.at(thread_index).size());
			thread_index++;
		}}
	}

	void fill_data_generate () {
		msg = generate_random_char_vector(base_msg_len);
		splitted_msg = split_char_vector(msg, threads_num);
	}
	void fill_data_variant () {
		size_t array_index = args_map.at(base_msg_len);
		auto variant_arr = dynamic_get(array_index, global_buffers);

		chunk_visitor cv(threads_num);
		splitted_msg = boost::apply_visitor(cv, variant_arr);
		msg = thread_safe::concentrate_vector(splitted_msg);
	}
	void fill_data_variant_chunks (size_t chunk_size) {
		size_t array_index = args_map.at(base_msg_len);
		auto variant_arr = dynamic_get(array_index, global_buffers);

		chunk_visitor cv(threads_num);
		splitted_msg = boost::apply_visitor(cv, variant_arr);
		chunk_splitted_msg(chunk_size);
		msg = thread_safe::concentrate_vector(splitted_msg);
	}

	size_t base_msg_len;
	size_t threads_num;
	static std::atomic<bool> m_data_filled;
	static std::unique_ptr<data_container> m_m;

	static std::mutex mtx;
	static std::mutex mtx_clean;
	static std::mutex mtx_fill_data;

};
std::mutex data_container::mtx;
std::mutex data_container::mtx_clean;
std::mutex data_container::mtx_fill_data;
std::array<std::atomic<bool>,Max_Threads> data_container::if_finish;
std::atomic<bool> data_container::m_data_filled(false);
std::unique_ptr<data_container> data_container::m_m(nullptr);

#endif // DATA_CONTAINER_HPP
