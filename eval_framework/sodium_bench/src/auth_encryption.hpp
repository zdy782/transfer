#ifndef AUTH_ENCRYPTION_HPP
#define AUTH_ENCRYPTION_HPP

#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include "crypto_functions.hpp"
#include "utils.hpp"


static void BM_crypto_box_auth_encryption(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}
	cryptobox_keypair alice_keys (generate_kyepair());
	cryptobox_keypair bob_keys (generate_kyepair());

	size_t msg_length = state.range(0);
	std::vector<char> msg(generate_random_char_vector(msg_length));
	std::vector<char> cipher(msg_length+crypto_box_MACBYTES);

	std::array<unsigned char, crypto_box_NONCEBYTES> nonce;

	while (state.KeepRunning()) {
		nonce = generate_random_array<unsigned char, crypto_box_NONCEBYTES>();

		cipher = cryptobox_encrypt(msg,nonce,bob_keys.public_key,alice_keys.secret_key);
	}
}

static void BM_crypto_box_auth_encrypt_decrypt(benchmark::State& state) {
	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}
	cryptobox_keypair alice_keys (generate_kyepair());
	cryptobox_keypair bob_keys (generate_kyepair());

	size_t msg_length = state.range(0);
	std::vector<char> msg(generate_random_char_vector(msg_length));
	std::vector<char> cipher(msg_length+crypto_box_MACBYTES);

	std::array<unsigned char, crypto_box_NONCEBYTES> nonce;

	while (state.KeepRunning()) {
		nonce = generate_random_array<unsigned char,crypto_box_NONCEBYTES>();

		cipher = cryptobox_encrypt(msg,nonce,bob_keys.public_key,alice_keys.secret_key);
		std::vector<char> check_decrypt = cryptobox_decrypt(cipher,nonce,alice_keys.public_key,bob_keys.secret_key);
		assert(msg == check_decrypt && "fail to decrypt cipher");
	}
}


#endif // AUTH_ENCRYPTION_HPP
