#ifndef ENCRYPTION_HPP
#define ENCRYPTION_HPP

/**
 * Encryption/Decryption, XSalsa20 benchmark:
 * doc source: https://download.libsodium.org/doc/advanced/xsalsa20.html
 * XSalsa20 is a stream cipher based upon Salsa20 but with a much longer nonce: 192 bits instead of 64 bits.
 * XSalsa20 uses a 256-bit key as well as the first 128 bits of the nonce in order to compute a subkey.
 * This subkey, as well as the remaining 64 bits of the nonce, are the parameters of the Salsa20 function
 * used to actually generate the stream.
 * Like Salsa20, XSalsa20 is immune to timing attacks and provides its own 64-bit block counter to avoid incrementing
 * the nonce after each block.
 * But with XSalsa20's longer nonce, it is safe to generate nonces using randombytes_buf() for every message encrypted
 * with the same key without having to worry about a collision.
 * Sodium exposes XSalsa20 with 20 rounds as the crypto_stream operation.
 */

#include <sodium.h>
#include "crypto_functions.hpp"
#include "utils.hpp"
#include "data_container.hpp"


static void BM_simple_XSalsa20_encryption(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}

	size_t msg_length = state.range(0);
	std::vector<char> msg(generate_random_char_vector(msg_length));
	std::vector<char> cipher(msg_length);

	std::array<unsigned char, crypto_stream_xsalsa20_NONCEBYTES> nonce;
	std::array<unsigned char, crypto_stream_KEYBYTES> key (generate_random_array<unsigned char,crypto_stream_KEYBYTES>());

	while (state.KeepRunning()) {
		nonce = generate_random_array<unsigned char,crypto_stream_xsalsa20_NONCEBYTES>();
		cipher = xsalsa_crypto_stream_xor(msg, nonce, key);
	}
}

static void BM_simple_XSalsa20_encryption_and_decryption(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}

	size_t msg_length = state.range(0);
	std::vector<char> msg(generate_random_char_vector(msg_length));
	std::vector<char> cipher(msg_length);

	std::array<unsigned char, crypto_stream_xsalsa20_NONCEBYTES> nonce;
	std::array<unsigned char, crypto_stream_KEYBYTES> key (generate_random_array<unsigned char, crypto_stream_KEYBYTES>());

	while (state.KeepRunning()) {
		nonce = generate_random_array<unsigned char,crypto_stream_xsalsa20_NONCEBYTES>();
		cipher = xsalsa_crypto_stream_xor(msg, nonce, key);
		std::vector<char> check = xsalsa_crypto_stream_xor(cipher, nonce, key);
		assert(msg == check && "fail to decrypt cipher");
	}
}

static void BM_multithread_xsalsa_sym_encrypt(benchmark::State& state) {

	data_container &mbag = data_container::get_m(state.threads(), state.range(0), data_type::variant);
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	if (state.thread_index() == 0) {
		// Setup code here.
	}

	// do nothing if thread is not necessary
	if (mbag.if_finish.at(state.thread_index()) == true) {
		state.SkipWithError("To many threads for small data");
	}

	start = std::chrono::high_resolution_clock::now();
	while (state.KeepRunning()) {

		mbag.setup_data(data_type::variant);
		mbag.cipher.at(state.thread_index()) = xsalsa_crypto_stream_xor(mbag.splitted_msg.at(state.thread_index()),
																	  mbag.nonce,
																	  mbag.alice_keys.secret_key);

		mbag.check_result.at(state.thread_index()) = xsalsa_crypto_stream_xor(mbag.cipher.at(state.thread_index()),
																	  mbag.nonce,
																	  mbag.alice_keys.secret_key);
		benchmark::ClobberMemory();
	}
	mbag.if_finish.at(state.thread_index()) = true;
	end = std::chrono::high_resolution_clock::now();
	auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	// state.threads() twice because SetIterationTime seems to divide be one. Same with state.iterations()
	auto all_time_cost = elapsed_seconds + thread_cost*state.threads()*(state.threads()-1)*state.iterations();
	state.SetIterationTime(all_time_cost.count());

	if (state.thread_index() == 0) {
		bool all_finish = false;
		while(!all_finish) {
			for (const auto &b : mbag.if_finish) {
				all_finish = true;
				if(b == false) {
					all_finish = false;
				}
			}
			std::this_thread::yield();
		}

		// main check
		if (mbag.get_base_msg() != mbag.get_result_msg()) {
			std::cout << "FAIL TO CORRECTLY DECRYPT ;/\n";
			std::cout << "L:" << mbag.get_base_msg() << '\n';
			std::cout << "R:" << mbag.get_result_msg() << '\n';
		}
		mbag.clear_container();
	}
}

static void BM_multithread_xsalsa_sym_encrypt_nothreadcost(benchmark::State& state) {

	data_container &mbag = data_container::get_m(state.threads(), state.range(0), data_type::variant);
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	if (state.thread_index() == 0) {
		// Setup code here.
	}

	// do nothing if thread is not necessary
	if (mbag.if_finish.at(state.thread_index()) == true) {
		state.SkipWithError("To many threads for small data");
	}

	start = std::chrono::high_resolution_clock::now();
	while (state.KeepRunning()) {

		mbag.setup_data(data_type::variant);
		mbag.cipher.at(state.thread_index()) = xsalsa_crypto_stream_xor(mbag.splitted_msg.at(state.thread_index()),
																	  mbag.nonce,
																	  mbag.alice_keys.secret_key);

		mbag.check_result.at(state.thread_index()) = xsalsa_crypto_stream_xor(mbag.cipher.at(state.thread_index()),
																	  mbag.nonce,
																	  mbag.alice_keys.secret_key);
		benchmark::ClobberMemory();
	}
	mbag.if_finish.at(state.thread_index()) = true;
	end = std::chrono::high_resolution_clock::now();
	auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	state.SetIterationTime(elapsed_seconds.count());

	if (state.thread_index() == 0) {
		bool all_finish = false;
		while(!all_finish) {
			for (const auto &b : mbag.if_finish) {
				all_finish = true;
				if(b == false) {
					all_finish = false;
				}
			}
			std::this_thread::yield();
		}

		// main check
		if (mbag.get_base_msg() != mbag.get_result_msg()) {
			std::cout << "FAIL TO CORRECTLY DECRYPT ;/\n";
			std::cout << "L:" << mbag.get_base_msg() << '\n';
			std::cout << "R:" << mbag.get_result_msg() << '\n';
		}
		mbag.clear_container();
	}
}

static void BM_multithread_xsalsa_sym_encrypt_chunks8k(benchmark::State& state) {

	data_container &mbag = data_container::get_m(state.threads(), state.range(0), data_type::variant);
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	if (state.thread_index() == 0) {
		// Setup code here.
	}

	// do nothing if thread is not necessary
	if (mbag.if_finish.at(state.thread_index()) == true) {
		state.SkipWithError("To many threads for small data");
	}

	start = std::chrono::high_resolution_clock::now();
	while (state.KeepRunning()) {

		mbag.setup_data(data_type::variant_chunk8k);
		{ size_t index = 0;
		for(auto &part : mbag.splitted_msg_chunks.at(state.thread_index())) {
			mbag.chunk_cipher.at(state.thread_index()).at(index) =
					xsalsa_crypto_stream_xor(part,
											 mbag.nonce,
											 mbag.alice_keys.secret_key);
			index++;
		}}
		{ size_t index = 0;
		for(auto &part : mbag.chunk_cipher.at(state.thread_index())) {
			mbag.chunk_result.at(state.thread_index()).at(index) =
					xsalsa_crypto_stream_xor(part,
											 mbag.nonce,
											 mbag.alice_keys.secret_key);
			index++;
		}}

		benchmark::ClobberMemory();
	}
	mbag.if_finish.at(state.thread_index()) = true;
	end = std::chrono::high_resolution_clock::now();
	auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	state.SetIterationTime(elapsed_seconds.count());

	if (state.thread_index() == 0) {
		bool all_finish = false;
		while(!all_finish) {
			for (const auto &b : mbag.if_finish) {
				all_finish = true;
				if(b == false) {
					all_finish = false;
				}
			}
			std::this_thread::yield();
		}

		// main check
		std::string concentrated_result;
		for(auto &part : mbag.chunk_result) {
			concentrated_result += char_vector_tostring(thread_safe::concentrate_vector(part),false);
		}

		if (mbag.get_base_msg() != concentrated_result) {
			std::cout << "FAIL TO CORRECTLY DECRYPT ;/\n";
			std::cout << "L:" << mbag.get_base_msg() << '\n';
			std::cout << "R:" << concentrated_result << '\n';
		}
		mbag.clear_container();

	}
}

#endif // ENCRYPTION_HPP
