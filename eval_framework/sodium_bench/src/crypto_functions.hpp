#ifndef CRYPTO_FUNCTIONS_HPP
#define CRYPTO_FUNCTIONS_HPP

#include <sodium.h>
#include <array>
#include <vector>
#include <stdexcept>

struct cryptobox_keypair {
	std::array<unsigned char, crypto_box_PUBLICKEYBYTES> public_key;
	std::array<unsigned char, crypto_box_SECRETKEYBYTES> secret_key;
};

cryptobox_keypair generate_kyepair();

std::vector<char> cryptobox_encrypt(std::vector<char> &msg,
                                    std::array<unsigned char, crypto_box_NONCEBYTES> &nonce,
                                    std::array<unsigned char, crypto_box_PUBLICKEYBYTES> &public_key,
                                    std::array<unsigned char, crypto_box_SECRETKEYBYTES> &secret_key);

std::vector<char> cryptobox_decrypt (std::vector<char> &cipher,
                                    std::array<unsigned char, crypto_box_NONCEBYTES> &nonce,
                                    std::array<unsigned char, crypto_box_PUBLICKEYBYTES> &public_key,
                                    std::array<unsigned char, crypto_box_SECRETKEYBYTES> &secret_key);

std::vector<unsigned char> secretbox_easy_encrypt(std::vector<unsigned char> &msg,
                                         std::array<unsigned char, crypto_secretbox_NONCEBYTES> &nonce,
                                         std::array<unsigned char, crypto_secretbox_KEYBYTES> &key);

std::vector<unsigned char> secretbox_easy_decrypt(std::vector<unsigned char> &cipher,
                                         std::array<unsigned char, crypto_secretbox_NONCEBYTES> &nonce,
                                         std::array<unsigned char, crypto_secretbox_KEYBYTES> &key);

std::vector<char> xsalaxa_crypto_stream(const size_t clen,
                                        std::array<unsigned char, crypto_stream_NONCEBYTES> &nonce,
                                        std::array<unsigned char, crypto_stream_KEYBYTES> &key);

std::vector<char> xsalsa_crypto_stream_xor(std::vector<char> &msg,
                                           std::array<unsigned char, crypto_stream_NONCEBYTES> &nonce,
                                           std::array<unsigned char, crypto_stream_KEYBYTES> &key);

std::array<unsigned char,crypto_onetimeauth_BYTES> poly1305_onetime_auth(std::vector<char> msg,
                                                                         std::array<unsigned char,crypto_onetimeauth_KEYBYTES> key);

int poly1305_onetime_auth_verify(std::array<unsigned char,crypto_onetimeauth_BYTES> out,
                                 std::vector<char> msg,
                                 std::array<unsigned char,crypto_onetimeauth_KEYBYTES> key);




#endif // CRYPTO_FUNCTIONS_HPP
