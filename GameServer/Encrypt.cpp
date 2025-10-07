#include "pch.h"
#include "Encrypt.h"


// 1) X25519 키쌍 생성 (EVP_PKEY)
EVP_PKEY* x25519_generate_key() {
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, NULL);
    if (!pctx) return nullptr;
    if (EVP_PKEY_keygen_init(pctx) <= 0) { EVP_PKEY_CTX_free(pctx); return nullptr; }

    EVP_PKEY* pkey = nullptr;
    if (EVP_PKEY_keygen(pctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(pctx);
        return nullptr;
    }
    EVP_PKEY_CTX_free(pctx);
    return pkey; // caller must EVP_PKEY_free()
}

// 2) 공개키(32바이트) 추출
bool x25519_get_public(const EVP_PKEY* pkey, std::vector<uint8_t>& pub_out) {
    size_t len = 0;
    // 먼저 길이 질의
    if (EVP_PKEY_get_raw_public_key(pkey, NULL, &len) <= 0) return false;
    pub_out.resize(len);
    if (EVP_PKEY_get_raw_public_key(pkey, pub_out.data(), &len) <= 0) return false;
    return true;
}

// 3) 비밀 공유 도출 (shared secret)
bool x25519_derive_shared(EVP_PKEY* mykey, const std::vector<uint8_t>& peer_pub, std::vector<uint8_t>& shared) {
    EVP_PKEY* peer = nullptr;
    // peer 공개키를 EVP_PKEY 형태로 생성 (raw public -> EVP_PKEY)
    EVP_PKEY_CTX* ctx = nullptr;
    bool ok = false;
    size_t secret_len = 0;
    // Create an EVP_PKEY from raw public bytes
    EVP_PKEY* peerkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, NULL, peer_pub.data(), peer_pub.size());
    if (!peerkey) return false;

    ctx = EVP_PKEY_CTX_new(mykey, NULL);
    if (!ctx) { EVP_PKEY_free(peerkey); return false; }
    if (EVP_PKEY_derive_init(ctx) <= 0) goto done;
    if (EVP_PKEY_derive_set_peer(ctx, peerkey) <= 0) goto done;

    
    if (EVP_PKEY_derive(ctx, NULL, &secret_len) <= 0) goto done;
    shared.resize(secret_len);
    if (EVP_PKEY_derive(ctx, shared.data(), &secret_len) <= 0) goto done;
    shared.resize(secret_len);
    ok = true;

done:
    if (ctx) EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(peerkey);
    return ok;
}

// 4) HKDF (SHA256)으로 shared secret -> AES-256 키(32바이트) 파생
bool hkdf_sha256(const std::vector<uint8>& ikm, const std::vector<uint8>& salt, const std::vector<uint8> info, std::vector<uint8>& okm, size_t okm_len)
{
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
    if (!pctx) return false;
    if (EVP_PKEY_derive_init(pctx) <= 0) { EVP_PKEY_CTX_free(pctx); return false; }
    if (EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha256()) <= 0) { EVP_PKEY_CTX_free(pctx); return false; }

    if (!salt.empty()) {
        if (EVP_PKEY_CTX_set1_hkdf_salt(pctx, salt.data(), salt.size()) <= 0) { EVP_PKEY_CTX_free(pctx); return false; }
    }
    if (EVP_PKEY_CTX_set1_hkdf_key(pctx, ikm.data(), ikm.size()) <= 0) { EVP_PKEY_CTX_free(pctx); return false; }
    if (!info.empty()) {
        if (EVP_PKEY_CTX_add1_hkdf_info(pctx, info.data(), info.size()) <= 0) { EVP_PKEY_CTX_free(pctx); return false; }
    }

    okm.resize(okm_len);
    size_t len = okm_len;
    if (EVP_PKEY_derive(pctx, okm.data(), &len) <= 0) { EVP_PKEY_CTX_free(pctx); return false; }
    EVP_PKEY_CTX_free(pctx);
    return true;
}



// 간단한 AES-GCM 암호화(예시, 256-bit key)
#include <openssl/evp.h>
bool aes256_gcm_encrypt(const std::vector<uint8_t>& key, SendBufferRef& sendBuffer)
{
    unsigned char* message = reinterpret_cast<unsigned char*>(sendBuffer->Buffer() + sizeof(PacketHeader));
    int len = sendBuffer->WriteSize() - sizeof(PacketHeader);

    // 12-byte IV (nonce)
    std::array<uint8_t, 12> iv_out;
    if (!RAND_bytes(iv_out.data(), iv_out.size())) return false;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) { EVP_CIPHER_CTX_free(ctx); return false; }
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_out.size(), NULL)) { EVP_CIPHER_CTX_free(ctx); return false; }
    if (!EVP_EncryptInit_ex(ctx, NULL, NULL, key.data(), iv_out.data())) { EVP_CIPHER_CTX_free(ctx); return false; }

    int outlen = 0;
    if (!EVP_EncryptUpdate(ctx, message, &outlen, message, len)) { EVP_CIPHER_CTX_free(ctx); return false; }

    int tmplen = 0;
    if (!EVP_EncryptFinal_ex(ctx, message + outlen, &tmplen)) { EVP_CIPHER_CTX_free(ctx); return false; }
    outlen += tmplen;

    std::array<uint8_t, 16> tag_out;
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, tag_out.size(), tag_out.data())) { EVP_CIPHER_CTX_free(ctx); return false; }

    EVP_CIPHER_CTX_free(ctx);

    

    PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
    header->size = sizeof(PacketHeader) + outlen;
    header->encrypted = 1;
    memcpy(header->iv.data(), iv_out.data(), iv_out.size());
    memcpy(header->tag.data(), tag_out.data(), tag_out.size());

    return true;
}
int aes256_gcm_decrypt(const std::vector<uint8_t>& key, BYTE* sendBuffer)
{
    PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer);
    BYTE* cypted_data = sendBuffer + sizeof(PacketHeader);
    int cypted_data_len = static_cast<int>(header->size) - static_cast<int>(sizeof(PacketHeader));

    if (header->encrypted == 0) {
        // 이미 평문
        return -1;
    }
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -1;
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) { EVP_CIPHER_CTX_free(ctx); return -1; }
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, header->iv.size(), NULL)) { EVP_CIPHER_CTX_free(ctx); return -1; }
    if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key.data(), header->iv.data())) { EVP_CIPHER_CTX_free(ctx); return -1; }

    
    int outlen = 0;
    if (!EVP_DecryptUpdate(ctx, cypted_data, &outlen, cypted_data, cypted_data_len)) { EVP_CIPHER_CTX_free(ctx); return -1; }
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, header->tag.size(), (void*)header->tag.data())) { EVP_CIPHER_CTX_free(ctx); return -1; }

    int finalplus = 0;
    int ret = EVP_DecryptFinal_ex(ctx, cypted_data + outlen, &finalplus);
    EVP_CIPHER_CTX_free(ctx);
    outlen += finalplus;
    //header->size = sizeof(PacketHeader) + outlen;
    return (ret > 0) ? outlen : -1;
}

bool make_aes256_key(const std::vector<uint8>& ikm, std::vector<uint8>& okm)
{
    std::vector<uint8_t> salt = {}; // 선택적 - can be random or session-specific
    std::vector<uint8_t> info = { 'g','a','m','e','-','s','e','s','s','i','o','n' };
    
    if (!hkdf_sha256(ikm, salt, info, okm, 32)) { return false; }
    return true;

}
