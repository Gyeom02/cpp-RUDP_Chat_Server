#pragma once
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/kdf.h>
#include <openssl/rand.h>   
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

static std::string to_hex(const std::vector<uint8_t>& b) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (auto v : b) oss << std::setw(2) << (int)v;
    return oss.str();
}
EVP_PKEY* x25519_generate_key();
bool x25519_get_public(const EVP_PKEY* pkey, std::vector<uint8>& pub_out);
bool x25519_derive_shared(EVP_PKEY* mykey, const std::vector<uint8>& peer_pub, std::vector<uint8>& shared);
bool hkdf_sha256(const std::vector<uint8>& ikm, const std::vector<uint8>& salt, const std::vector<uint8> info, std::vector<uint8>& okm, size_t okm_len);

bool aes256_gcm_encrypt(const std::vector<uint8>& key, SendBufferRef& sendBuffer);
int aes256_gcm_decrypt(const std::vector<uint8>& key, BYTE* sendBuffer);

bool make_aes256_key(const std::vector<uint8>& ikm, std::vector<uint8>& okm);


class X25519KeyPair {
private:
    EVP_PKEY* pkey = nullptr;

public:
    X25519KeyPair() = default;

    // 복사 금지 (EVP_PKEY는 얕은 복사 금지)
    X25519KeyPair(const X25519KeyPair&) = delete;
    X25519KeyPair& operator=(const X25519KeyPair&) = delete;

    // 이동 허용 (예: std::move)
    X25519KeyPair(X25519KeyPair&& other) noexcept {
        pkey = other.pkey;
        other.pkey = nullptr;
    }

    X25519KeyPair& operator=(X25519KeyPair&& other) noexcept {
        if (this != &other) {
            freeKey();
            pkey = other.pkey;
            other.pkey = nullptr;
        }
        return *this;
    }

    ~X25519KeyPair() {
        freeKey();
    }

    void freeKey() {
        if (pkey) {
            EVP_PKEY_free(pkey);
            pkey = nullptr;
        }
    }

    bool generate() {
        freeKey();

        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, NULL);
        if (!ctx) return false;

        if (EVP_PKEY_keygen_init(ctx) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            return false;
        }

        if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            return false;
        }

        EVP_PKEY_CTX_free(ctx);
        return true;
    }

    EVP_PKEY* get() const { return pkey; }

    /*bool getPublicKey(std::vector<uint8_t>& pub_out) const {
        if (!pkey) return false;
        size_t len = 0;
        if (EVP_PKEY_get_raw_public_key(pkey, NULL, &len) <= 0) return false;
        pub_out.resize(len);
        if (EVP_PKEY_get_raw_public_key(pkey, pub_out.data(), &len) <= 0) return false;
        return true;
    }

    bool getPrivateKey(std::vector<uint8_t>& priv_out) const {
        if (!pkey) return false;
        size_t len = 0;
        if (EVP_PKEY_get_raw_private_key(pkey, NULL, &len) <= 0) return false;
        priv_out.resize(len);
        if (EVP_PKEY_get_raw_private_key(pkey, priv_out.data(), &len) <= 0) return false;
        return true;
    }*/
};