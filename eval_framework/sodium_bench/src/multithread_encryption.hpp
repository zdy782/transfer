#ifndef MULTITHREAD_ENCRYPTION_HPP
#define MULTITHREAD_ENCRYPTION_HPP

#include <iostream>
#include <thread>
#include "utils.hpp"
#include "auth_encryption.hpp"
#include "data_container.hpp"


//static void BM_threaded_onetimeAuth(benchmark::State& state) {
//	while (state.KeepRunning()) {

//		static_container &mbag = static_container::get_m(state.threads(), state.range(0));

//		// not run thread if is not necessary
//		if (mbag.if_finish.at(state.thread_index()) == true) {
//			return;
//		}
//		auto start = std::chrono::high_resolution_clock::now();

//		mbag.cipher.at(state.thread_index()) =
//		crypto_onetimeauth(reinterpret_cast<const unsigned char *> mbag.cipher.at(state.thread_index()), reinterpret_cast<const unsigned char *>(msg.c_str()), msg_len, key);


//		auto end = std::chrono::high_resolution_clock::now();

//		mbag.if_finish.at(state.thread_index()) = true;

//		auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
//		state.SetIterationTime(elapsed_seconds.count());
//	}
//}

static void BM_threaded_auth_encrypt(benchmark::State& state) {

	data_container &mbag = data_container::get_m(state.threads(), state.range(0));
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

		mbag.setup_data(data_type::generate);
		mbag.cipher.at(state.thread_index()) = cryptobox_encrypt(mbag.splitted_msg.at(state.thread_index()),
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

		// Setup code here.
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

static void BM_threaded_auth_encrypt_decrypt(benchmark::State& state) {

	data_container &mbag = data_container::get_m(state.threads(), state.range(0));
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

		mbag.setup_data(data_type::generate);
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
		std::vector<char> msg_concentrate;
		msg_concentrate.reserve(mbag.msg.size());
		for(auto &i : mbag.check_result) {
			msg_concentrate.insert(msg_concentrate.end(), i.begin(), i.end());
		}

		// main check
		if (mbag.msg != msg_concentrate) {
			std::cout << "FAIL TO CORRECTLY DECRYPT ;/\n";
		}
		mbag.clear_container();
	}
}

#endif // MULTITHREAD_ENCRYPTION_HPP
