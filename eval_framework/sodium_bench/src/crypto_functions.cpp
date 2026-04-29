#include "crypto_functions.hpp"

// pub key auth encryption

cryptobox_keypair generate_kyepair() {
	cryptobox_keypair keys;
	crypto_box_keypair(keys.public_key.data(),
	                   keys.secret_key.data());
	return keys;
}


std::vector<char> cryptobox_encrypt(std::vector<char> &msg,
                                    std::array<unsigned char, crypto_box_NONCEBYTES> &nonce,
                                    std::array<unsigned char, crypto_box_PUBLICKEYBYTES> &public_key,
                                    std::array<unsigned char, crypto_box_SECRETKEYBYTES> &secret_key) {

	std::vector<char> cipher(msg.size()+crypto_box_MACBYTES);

	if (crypto_box_easy(reinterpret_cast<unsigned char *>(cipher.data()),
	                    reinterpret_cast<unsigned char *>(msg.data()),
	                    msg.size(),
	                    nonce.data(),
	                    public_key.data(),
	                    secret_key.data()) != 0) {
		throw std::runtime_error{"Fail to auth encrypt msg"};
	}
	return cipher;
}

std::vector<char> cryptobox_decrypt(std::vector<char> &cipher,
                                    std::array<unsigned char, crypto_box_NONCEBYTES> &nonce,
                                    std::array<unsigned char, crypto_box_PUBLICKEYBYTES> &public_key,
                                    std::array<unsigned char, crypto_box_SECRETKEYBYTES> &secret_key) {

	std::vector<char> msg(cipher.size()-crypto_box_MACBYTES);
	if (crypto_box_open_easy(reinterpret_cast<unsigned char *>(msg.data()),
	                         reinterpret_cast<unsigned char *>(cipher.data()),
	                         cipher.size(),
	                         nonce.data(),
	                         public_key.data(),
	                         secret_key.data()) != 0) {
		throw std::runtime_error{"Fail to auth decrypt msg"};
	}
	return msg;
}


// sym key auth encryption

std::vector<unsigned char> secretbox_easy_encrypt(std::vector<unsigned char> &msg,
                                         std::array<unsigned char, crypto_secretbox_NONCEBYTES> &nonce,
                                         std::array<unsigned char, crypto_secretbox_KEYBYTES> &key) {

	// cipher+auth-tag
	std::vector<unsigned char> cipher(msg.size()+crypto_secretbox_MACBYTES);

	if (crypto_secretbox_easy(cipher.data(),
	                          msg.data(),
	                          msg.size(),
	                          nonce.data(),
	                          key.data() ) != 0) {
		throw std::runtime_error{"Fail to sym auth encrypt - message forged!"};
	};

	return cipher;
}

std::vector<unsigned char> secretbox_easy_decrypt(std::vector<unsigned char> &cipher,
                                         std::array<unsigned char, crypto_secretbox_NONCEBYTES> &nonce,
                                         std::array<unsigned char, crypto_secretbox_KEYBYTES> &key) {

	std::vector<unsigned char> msg(cipher.size()-crypto_secretbox_MACBYTES);
	if (crypto_secretbox_open_easy(msg.data(),
	                               cipher.data(),
	                               cipher.size(),
	                               nonce.data(),
	                               key.data()) != 0) {
		throw std::runtime_error{"Fail to sym auth decrypt - message forged!"};
	}
	return msg;
}

// stream sym encryption

std::vector<char> xsalaxa_crypto_stream(const size_t clen,
                                        std::array<unsigned char, crypto_stream_NONCEBYTES> &nonce,
                                        std::array<unsigned char, crypto_stream_KEYBYTES> &key) {

	std::vector<char> c(clen);
	crypto_stream(reinterpret_cast<unsigned char *>(c.data()),
	              clen,
	              nonce.data(),
	              key.data());

	return c;
}

std::vector<char> xsalsa_crypto_stream_xor(std::vector<char> &msg,
                                           std::array<unsigned char, crypto_stream_NONCEBYTES> &nonce,
                                           std::array<unsigned char, crypto_stream_KEYBYTES> &key) {

	size_t msg_len = msg.size();
	std::vector<char> c(msg_len);

	crypto_stream_xor(reinterpret_cast<unsigned char *>(c.data()),
	                  reinterpret_cast<unsigned char *>(msg.data()),
	                  msg_len,
	                  nonce.data(),
	                  key.data());

	return c;
}

// authentication

std::array<unsigned char, crypto_onetimeauth_BYTES> poly1305_onetime_auth(std::vector<char> msg,
                                                                          std::array<unsigned char, crypto_onetimeauth_KEYBYTES> key){

	std::array<unsigned char,crypto_onetimeauth_BYTES> out;
	crypto_onetimeauth(out.data(),
	                   reinterpret_cast<unsigned char *>(msg.data()),
	                   msg.size(),
	                   key.data());
	return out;
}

int poly1305_onetime_auth_verify(std::array<unsigned char, crypto_onetimeauth_BYTES> out,
                                 std::vector<char> msg,
                                 std::array<unsigned char, crypto_onetimeauth_KEYBYTES> key) {

	return crypto_onetimeauth_verify(out.data(),
	                                 reinterpret_cast<unsigned char *>(msg.data()),
	                                 msg.size(),
	                                 key.data());
}

