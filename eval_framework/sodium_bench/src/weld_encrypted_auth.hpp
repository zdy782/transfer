#ifndef WELD_ENCRYPTED_AUTH_HPP
#define WELD_ENCRYPTED_AUTH_HPP

#include "crypto_functions.hpp"
#include "weld_arch.hpp"
#include "global_buffers.hpp"


static void BM_weld_packets_eat(benchmark::State& state) {

	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}

	size_t dst_num = state.range(0);
	auto packets = generate_random_packets(2000,dst_num);

	weld_manager_continous m_man(dst_num);

	while (state.KeepRunning()) {
		benchmark::DoNotOptimize(m_man);
		auto start = std::chrono::high_resolution_clock::now();

		benchmark::DoNotOptimize(m_man);

		m_man.eat(std::move(packets));

		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		state.SetIterationTime(elapsed_seconds.count());

		benchmark::ClobberMemory();
	}
}

static void BM_weld_encrypt_decrypt(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}

	auto nonce = generate_random_array<unsigned char,crypto_secretbox_NONCEBYTES>();
	auto sym_key = generate_random_array<unsigned char,crypto_secretbox_KEYBYTES>();

	size_t dst_num = state.range(1);
	auto packets = generate_random_packets((state.range(0)/64),dst_num);
	weld_manager_continous m_man(dst_num);
	m_man.eat(std::move(packets));

	//m_man.print_status();

	while (state.KeepRunning()) {
		benchmark::DoNotOptimize(m_man);
		auto start = std::chrono::high_resolution_clock::now();

		//std::cout << "before encrypt\n"; m_man.print_status();
		m_man.encrypt_all_buffers(nonce, sym_key);
		//std::cout << "after encrypt\n"; m_man.print_status();
		m_man.decrypt_all_buffers(nonce, sym_key);
		//std::cout << "after decrypt\n"; m_man.print_status();

		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		state.SetIterationTime(elapsed_seconds.count());

		benchmark::ClobberMemory();
	}

	//m_man.print_status();
}


static void BM_weld_encrypt(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}

	auto nonce = generate_random_array<unsigned char,crypto_secretbox_NONCEBYTES>();
	auto sym_key = generate_random_array<unsigned char,crypto_secretbox_KEYBYTES>();

	size_t dst_num = state.range(1);
	auto packets = chunk_custom_buffors(random_buf, state.range(0), dst_num);
	weld_manager_continous m_man(dst_num);
	m_man.eat(std::move(packets));

	//m_man.print_status();

	while (state.KeepRunning()) {
		benchmark::DoNotOptimize(m_man);
		auto start = std::chrono::high_resolution_clock::now();


		//std::cout << "before encrypt\n"; m_man.print_status();
		m_man.encrypt_all_buffers(nonce, sym_key);
		//std::cout << "after encrypt\n"; m_man.print_status();
		//m_man.decrypt_all_buffers(nonce, sym_key);
		//std::cout << "after decrypt\n"; m_man.print_status();


		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
		state.SetIterationTime(elapsed_seconds.count());

		benchmark::ClobberMemory();
	}

	//m_man.print_status();
}

#endif // WELD_ENCRYPTED_AUTH_HPP
