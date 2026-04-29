#ifndef HASHES_HPP
#define HASHES_HPP
#include <sodium.h>

static void BM_crypto_sha256(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}
	std::string msg (state.range(0),'x');
	size_t msg_len = state.range(0);
	unsigned char out[crypto_hash_sha256_BYTES];

	while (state.KeepRunning()) {

		crypto_hash_sha256(out, reinterpret_cast<const unsigned char *>(msg.c_str()), msg_len);
	}
}

static void BM_crypto_sha512(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}
	std::string msg (state.range(0),'x');
	size_t msg_len = state.range(0);
	unsigned char out[crypto_hash_sha512_BYTES];

	while (state.KeepRunning()) {

		crypto_hash_sha512(out, reinterpret_cast<const unsigned char *>(msg.c_str()), msg_len);
	}
}


#endif // HASHES_HPP
