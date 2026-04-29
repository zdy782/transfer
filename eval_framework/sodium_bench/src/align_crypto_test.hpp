#ifndef ALIGN_CRYPTO_TEST_HPP
#define ALIGN_CRYPTO_TEST_HPP

#include "global_buffers.hpp"
#include "crypto_functions.hpp"

static void BM_align_test(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}

	auto start_byte =  state.range(0);
	size_t bytes_to_encrypt = 1024*4;
	auto buffer_part = std::vector<unsigned char>(random_buf.begin()+start_byte,
	                                              random_buf.begin()+start_byte+bytes_to_encrypt);

	auto nonce = generate_random_array<unsigned char,crypto_secretbox_NONCEBYTES>();
	auto sym_key = generate_random_array<unsigned char,crypto_secretbox_KEYBYTES>();

	while (state.KeepRunning()) {
		auto start = std::chrono::high_resolution_clock::now();

		auto cipher = secretbox_easy_encrypt(buffer_part, nonce, sym_key);

		benchmark::DoNotOptimize(cipher);

		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		state.SetIterationTime(elapsed_seconds.count());

		benchmark::ClobberMemory();
	}
}


#endif // ALIGN_CRYPTO_TEST_HPP
