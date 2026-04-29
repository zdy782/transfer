#ifndef MULTITHREAD_RAM_ENCRYPTION_HPP
#define MULTITHREAD_RAM_ENCRYPTION_HPP


#include "auth_encryption.hpp"
#include "global_buffers.hpp"
#include "utils.hpp"
#include "data_container.hpp"


static void BM_staticData_encrypt_decrypt(benchmark::State& state) {

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

		// main check
		if (mbag.get_base_msg() != mbag.get_result_msg()) {
			std::cout << "FAIL TO CORRECTLY DECRYPT ;/\n";
			std::cout << "L:" << mbag.get_base_msg() << '\n';
			std::cout << "R:" << mbag.get_result_msg() << '\n';
		}
		mbag.clear_container();
	}
}

#endif // MULTITHREAD_RAM_ENCRYPTION_HPP
