#ifndef AUTHENTICATION_HPP
#define AUTHENTICATION_HPP

#endif // AUTHENTICATION_HPP

#include <sodium.h>
#include "crypto_functions.hpp"
#include "global_buffer_variant.hpp"
#include "data_container.hpp"

static void BM_crypto_manybuf_split(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}
	std::vector<std::string> string_vec;
	size_t msg_len = 64000/state.range(0);
	for (int i = 0; i < state.range(0); ++i) {
		string_vec.emplace_back(std::string(msg_len,static_cast<char>(rand())));
	}

	while (state.KeepRunning()) {
		unsigned char out[crypto_onetimeauth_BYTES];
		unsigned char key[crypto_onetimeauth_KEYBYTES];
		crypto_onetimeauth_state state;

		randombytes_buf(key, sizeof key);

		crypto_onetimeauth_init(&state, key);
		for(auto &str : string_vec) {
			crypto_onetimeauth_update(&state, reinterpret_cast<const unsigned char *>(str.c_str()), msg_len);
		}
		crypto_onetimeauth_final(&state, out);
	}
}

static void BM_crypto_manybuf_onetimeAuth(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}

	std::string msg01 (state.range(0),'x');
	std::string msg02 (state.range(0),'y');
	size_t msg_len01 = state.range(0);
	size_t msg_len02 = msg_len01;


	while (state.KeepRunning()) {
		unsigned char out[crypto_onetimeauth_BYTES];
		unsigned char key[crypto_onetimeauth_KEYBYTES];
		crypto_onetimeauth_state state;

		randombytes_buf(key, sizeof key);

		crypto_onetimeauth_init(&state, key);
		crypto_onetimeauth_update(&state, reinterpret_cast<const unsigned char *>(msg01.c_str()), msg_len01);
		crypto_onetimeauth_update(&state, reinterpret_cast<const unsigned char *>(msg02.c_str()), msg_len02);
		crypto_onetimeauth_final(&state, out);

	}
}

static void BM_crypto_single_onetimeAuth(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}
	std::string msg (state.range(0),'x');
	size_t msg_len = state.range(0);

	unsigned char out[crypto_onetimeauth_BYTES];
	unsigned char key[crypto_onetimeauth_KEYBYTES];

	while (state.KeepRunning()) {

		randombytes_buf(key, sizeof key);
		crypto_onetimeauth(out, reinterpret_cast<const unsigned char *>(msg.c_str()), msg_len, key);
	}
}

static void BM_crypto_single_onetimeAuth_and_verify(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}
	std::string msg (state.range(0),'x');
	size_t msg_len = state.range(0);

	while (state.KeepRunning()) {
		unsigned char out[crypto_onetimeauth_BYTES];
		unsigned char key[crypto_onetimeauth_KEYBYTES];

		randombytes_buf(key, sizeof key);
		crypto_onetimeauth(out, reinterpret_cast<const unsigned char *>(msg.c_str()), msg_len, key);

		if (crypto_onetimeauth_verify(out,  reinterpret_cast<const unsigned char *>(msg.c_str()), msg_len, key) != 0) {
				/* message forged! */
		}
		//state.SetBytesProcessed(state.bytes_processed()); // it seems to now work propertly, don't know why this function
												   //includes amount of iteration in his calculation
	}
}

static void BM_poly1305_authentication(benchmark::State& state) {

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

		mbag.onetime_auth_out.at(state.thread_index()) = poly1305_onetime_auth(mbag.splitted_msg.at(state.thread_index()),
																mbag.onetime_auth_key);
		if (poly1305_onetime_auth_verify(mbag.onetime_auth_out.at(state.thread_index()),
										 mbag.splitted_msg.at(state.thread_index()),
										 mbag.onetime_auth_key) != 0) {
			state.SkipWithError("Fail to verify poly1305_authenticated data!");
		}

		mbag.cipher.at(state.thread_index()) = cryptobox_encrypt(mbag.splitted_msg.at(state.thread_index()),
															   mbag.nonce,
															   mbag.bob_keys.public_key,
															   mbag.alice_keys.secret_key);

		mbag.check_result.at(state.thread_index()) = cryptobox_decrypt(mbag.cipher.at(state.thread_index()),
																	 mbag.nonce,
																	 mbag.bob_keys.public_key,
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
		mbag.clear_container();
	}
}
