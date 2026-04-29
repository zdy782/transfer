#include <iostream>

#include <cmath>
#include <benchmark/benchmark.h>
#include "utils.hpp"
#include "test/unit_tests.hpp"

#include "sort_tests.hpp"
#include "authentication.hpp"
#include "encryption.hpp"
#include "auth_encryption.hpp"
#include "multithread_encryption.hpp"
#include "multithread_ram_encryption.hpp"
#include "hashes.hpp"
#include "align_crypto_test.hpp"

//BENCHMARK(bubble_sort)->RangeMultiplier(2)->Range(2, 8<<8)->Complexity();
//BENCHMARK(std_sort)->RangeMultiplier(2)->Range(2, 8<<8)->Complexity();

// Register the function as a benchmark
//BENCHMARK(BM_crypto_manybuf_onetimeAuth)->Unit(benchmark::kMicrosecond);
//BENCHMARK(BM_crypto_manybuf_split)->RangeMultiplier(4)->Range(1, 8<<10);
//BENCHMARK(BM_crypto_manybuf_onetimeAuth)->RangeMultiplier(2)->Range(2, 8<<26);
//BENCHMARK(BM_crypto_single_onetimeAuth)->Unit(benchmark::kMicrosecond);

//BENCHMARK(BM_crypto_single_onetimeAuth_and_verify)->RangeMultiplier(2)->Range(2, 8<<25);
//BENCHMARK(BM_simple_XSalsa20_encryption)->RangeMultiplier(2)->Range(2, 8<<22);
//BENCHMARK(BM_simple_XSalsa20_encryption_and_decryption)->RangeMultiplier(2)->Range(2, 8<<22);

//BENCHMARK(BM_crypto_box_auth_encryption)->RangeMultiplier(2)->Range(2, 8<<16);
//BENCHMARK(BM_crypto_box_auth_encrypt_decrypt)->RangeMultiplier(2)->Range(2, 8<<16);


static void CustomArguments(benchmark::internal::Benchmark* b) {
  for (int i = 0; i <= 10; ++i)
	  b->Args({i, 2 << i });
}



constexpr size_t Min_inputArg = 64; // should be >= than thread num
//constexpr size_t Max_inputArg = 2<<6;
constexpr size_t Max_inputArg = max_buffer; // in "global_buffer_variant" file

//BENCHMARK(BM_crypto_single_onetimeAuth)->RangeMultiplier(2)->Range(Min_inputArg, Max_inputArg);
//BENCHMARK(BM_crypto_single_onetimeAuth_and_verify)->RangeMultiplier(2)->Range(Min_inputArg, Max_inputArg);

// anthenticated encryption

static void calc_thread_cost(benchmark::State& state) {
	int threads = 10000;
	auto start = std::chrono::high_resolution_clock::now();
	for (volatile int i = 0; i < threads; i++) {
		std::thread r([](){});
		r.join();
	}
	auto end = std::chrono::high_resolution_clock::now();
	thread_cost = (std::chrono::duration_cast<std::chrono::duration<double>>(end - start))/threads;
	while (state.KeepRunning()) {
		std::thread r([](){});
		r.join();
	}
}

//BENCHMARK(run_unit_tests);
//BENCHMARK(calc_thread_cost);

//BENCHMARK(BM_multithread_xsalsa_sym_encrypt_chunks8k)->RangeMultiplier(2)->Range(Min_inputArg, Max_inputArg)
//		  ->Threads(1)->Threads(2)->Threads(4)->Threads(8)
//		  ->Threads(12)->Threads(16)->Threads(32)->Threads(48)
//		  ->UseManualTime();

//BENCHMARK(BM_poly1305_authentication)->RangeMultiplier(2)->Range(Min_inputArg, Max_inputArg);
//		  ->Threads(1)->Threads(2)->Threads(4)->Threads(8)
//		  ->Threads(12)->Threads(16)->Threads(32)->Threads(48)
//		  ->UseManualTime();

//BENCHMARK(BM_multithread_xsalsa_sym_encrypt)->RangeMultiplier(2)->Range(Min_inputArg, Max_inputArg)
//		  ->Threads(1)->Threads(2)->Threads(4)->Threads(8)
//		  ->Threads(12)->Threads(16)->Threads(32)->Threads(48)
//		  ->UseManualTime();

//BENCHMARK(BM_multithread_xsalsa_sym_encrypt_nothreadcost)->RangeMultiplier(2)->Range(Min_inputArg, Max_inputArg)
//		  ->Threads(1)->Threads(2)->Threads(4)->Threads(8)
//		  ->Threads(12)->Threads(16)->Threads(32)->Threads(48)
//		  ->UseManualTime();

//BENCHMARK(BM_threaded_auth_encrypt)->RangeMultiplier(2)->Range(Min_inputArg, Max_inputArg)
//		  ->Threads(1)->Threads(2)->Threads(4)->Threads(8)
//		  ->Threads(12)->Threads(16)->Threads(32)->Threads(48)
//		  ->UseManualTime();

//// anthenticated encryption and decryption
//BENCHMARK(BM_threaded_auth_encrypt_decrypt)->RangeMultiplier(2)->Range(Min_inputArg, Max_inputArg)
//		  ->Threads(1)->Threads(2)->Threads(4)->Threads(8)
//		  ->Threads(12)->Threads(16)->Threads(32)->Threads(48)
//		  ->UseManualTime();

//BENCHMARK(BM_staticData_encrypt_decrypt)->RangeMultiplier(2)->Range(Min_inputArg,Max_inputArg)
//		  ->Threads(1)->Threads(2)->Threads(4)->Threads(8)
//		  ->Threads(12)->Threads(16)->Threads(32)->Threads(48)
//		  ->UseManualTime();


// poly1305 vs sha256
//BENCHMARK(BM_crypto_sha256)->RangeMultiplier(2)->Range(Min_inputArg,Max_inputArg);
//BENCHMARK(BM_crypto_sha512)->RangeMultiplier(2)->Range(Min_inputArg,Max_inputArg);
//BENCHMARK(BM_crypto_single_onetimeAuth)->RangeMultiplier(2)->Range(Min_inputArg, Max_inputArg);


#include "weld_encrypted_auth.hpp"
// Weld arch -
//BENCHMARK(BM_weld_encrypt)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(80)
//                          ->Arg(64)->Arg(124)->Arg(512)->Arg(1024)->Arg(4096)->Arg(65536)->UseManualTime();
static void buffs_args(benchmark::internal::Benchmark* b) {
	double multiplier = std::pow(2, 0.25);
	for (int pac_size = 64; pac_size <= 1024*1024*1024; pac_size *= multiplier)
		for (double bufs = 1; bufs <= 1024*32; bufs *= multiplier)
			b->Args({static_cast<int>(pac_size), static_cast<int>(bufs)});
}

//BENCHMARK(BM_weld_encrypt_decrypt)->UseManualTime()
//                          ->Apply(buffs_args);

BENCHMARK(BM_weld_encrypt)->UseManualTime()
                          ->Apply(buffs_args);

//BENCHMARK(BM_weld_simple_encrypt)->UseManualTime()
//                                 ->Apply(buffs_args);

//BENCHMARK(BM_weld_packets_eat)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(80)
//                          ->Arg(64)->Arg(124)->Arg(512)->Arg(1024)->Arg(4096)->Arg(65536)->UseManualTime();

//static void align_args(benchmark::internal::Benchmark* b) {
//	for (int bytes_start = 0; bytes_start <= 1024; bytes_start+=3)
//			b->Arg(static_cast<int>(bytes_start));
//}
//BENCHMARK(BM_align_test)->UseManualTime()
//                        ->Apply(align_args);

BENCHMARK_MAIN();

/*

static std::vector<std::pair<int,int>> buffs_args() {
	std::vector<std::pair<int,int>> local_args;
	double multiplier = std::pow(2, 2);
	for (int pac_size = 64; pac_size <= 1024*1024; pac_size *= multiplier)
		for (double bufs = 1; bufs <= 1024*16; bufs *= multiplier)
			local_args.emplace_back(std::make_pair(pac_size, bufs));

	return local_args;
}

int main (int argc, char** argv) {
	auto my_args = buffs_args();

	for(auto &i : my_args) {
		std::cout << i.first << " - " << i.second << "\n";
	}
	return 0;
}
*/
