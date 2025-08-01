/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018      Lee Clagett <https://github.com/vtnerd>
 * Copyright 2019      Howard Chu  <https://github.com/hyc>
 * Copyright 2018-2025 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2025 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XMRIG_JOB_H
#define XMRIG_JOB_H

#include <cstddef>
#include <cstdint>

#include "base/crypto/Algorithm.h"
#include "base/tools/Buffer.h"
#include "base/tools/String.h"


namespace xmrig {


class Job
{
public:
    // Max blob size is 84 (75 fixed + 9 variable), aligned to 96. https://github.com/xmrig/xmrig/issues/1 Thanks fireice-uk.
    // SECOR increase requirements for blob size: https://github.com/xmrig/xmrig/issues/913
    // Haven (XHV) offshore increases requirements by adding pricing_record struct (192 bytes) to block_header.
    // Round it up to 408 (136*3) for a convenient keccak calculation in OpenCL
    static constexpr const size_t kMaxBlobSize = 408;
    static constexpr const size_t kMaxSeedSize = 32;

    Job() = default;
    Job(bool nicehash, const Algorithm &algorithm, const String &clientId);

    inline Job(const Job &other)        { copy(other); }
    inline Job(Job &&other) noexcept    { move(std::move(other)); }

    ~Job() = default;

    bool isEqual(const Job &other) const;
    bool isEqualBlob(const Job &other) const;
    bool setBlob(const char *blob);
    bool setSeedHash(const char *hash);
    bool setTarget(const char *target);
    size_t nonceOffset() const;
    void setDiff(uint64_t diff);
    void setSigKey(const char *sig_key);

    inline bool isNicehash() const                      { return m_nicehash; }
    inline bool isDonate() const                        { return m_donate; }
    inline bool isValid() const                         { return (m_size > 0 && m_diff > 0) || !m_poolWallet.isEmpty(); }
    inline bool setId(const char *id)                   { return (m_id = id); }
    inline const Algorithm &algorithm() const           { return m_algorithm; }
    inline const Buffer &seed() const                   { return m_seed; }
    inline const String &clientId() const               { return m_clientId; }
    inline const String &extraNonce() const             { return m_extraNonce; }
    inline const String &id() const                     { return m_id; }
    inline const String &poolWallet() const             { return m_poolWallet; }
    inline const uint32_t *nonce() const                { return reinterpret_cast<const uint32_t*>(m_blob + nonceOffset()); }
    inline const uint8_t *blob() const                  { return m_blob; }
    inline size_t nonceSize() const                     { return (algorithm().family() == Algorithm::KAWPOW) ?  8 :  4; }
    inline size_t size() const                          { return m_size; }
    inline uint32_t *nonce()                            { return reinterpret_cast<uint32_t*>(m_blob + nonceOffset()); }
    inline uint32_t backend() const                     { return m_backend; }
    inline uint64_t diff() const                        { return m_diff; }
    inline uint64_t height() const                      { return m_height; }
    inline uint64_t nonceMask() const                   { return isNicehash() ? 0xFFFFFFULL : (nonceSize() == sizeof(uint64_t) ? (static_cast<uint64_t>(-1LL) >> (extraNonce().size() * 4)) : 0xFFFFFFFFULL); }
    inline uint64_t target() const                      { return m_target; }
    inline uint8_t *blob()                              { return m_blob; }
    inline uint8_t fixedByte() const                    { return *(m_blob + 42); }
    inline uint8_t index() const                        { return m_index; }
    inline void reset()                                 { m_size = 0; m_diff = 0; }
    inline void setDonate(bool donate)                  { m_donate = donate; }
    inline void setAlgorithm(const Algorithm::Id id)    { m_algorithm = id; }
    inline void setAlgorithm(const char *algo)          { m_algorithm = algo; }
    inline void setBackend(uint32_t backend)            { m_backend = backend; }
    inline void setClientId(const String &id)           { m_clientId = id; }
    inline void setExtraNonce(const String &extraNonce) { m_extraNonce = extraNonce; }
    inline void setHeight(uint64_t height)              { m_height = height; }
    inline void setIndex(uint8_t index)                 { m_index = index; }
    inline void setPoolWallet(const String &poolWallet) { m_poolWallet = poolWallet; }

#   ifdef XMRIG_PROXY_PROJECT
    inline char *rawBlob()                              { return m_rawBlob; }
    inline const char *rawBlob() const                  { return m_rawBlob; }
    inline const char *rawTarget() const                { return m_rawTarget; }
    inline const String &rawSeedHash() const            { return m_rawSeedHash; }
    inline const String &rawSigKey() const              { return m_rawSigKey; }
#   endif

    static inline uint64_t toDiff(uint64_t target)      { return target ? (0xFFFFFFFFFFFFFFFFULL / target) : 0; }

    inline bool operator!=(const Job &other) const      { return !isEqual(other); }
    inline bool operator==(const Job &other) const      { return isEqual(other); }
    inline Job &operator=(const Job &other)             { if (this != &other) { copy(other); } return *this; }
    inline Job &operator=(Job &&other) noexcept         { move(std::move(other)); return *this; }

#   ifdef XMRIG_PROXY_PROJECT
    inline bool hasViewTag() const                      { return m_hasViewTag; }

    void setSpendSecretKey(const uint8_t* key);
    void setMinerTx(const uint8_t* begin, const uint8_t* end, size_t minerTxEphPubKeyOffset, size_t minerTxPubKeyOffset, size_t minerTxExtraNonceOffset, size_t minerTxExtraNonceSize, const Buffer& minerTxMerkleTreeBranch, bool hasViewTag);
    void setViewTagInMinerTx(uint8_t view_tag);
    void setExtraNonceInMinerTx(uint32_t extra_nonce);
    void generateSignatureData(String& signatureData, uint8_t& view_tag) const;
    void generateHashingBlob(String& blob) const;
#   else
    inline const uint8_t* ephSecretKey() const { return m_hasMinerSignature ? m_ephSecretKey : nullptr; }

    inline void setEphemeralKeys(const uint8_t *pub_key, const uint8_t *sec_key)
    {
        m_hasMinerSignature = true;
        memcpy(m_ephPublicKey, pub_key, sizeof(m_ephSecretKey));
        memcpy(m_ephSecretKey, sec_key, sizeof(m_ephSecretKey));
    }

    void generateMinerSignature(const uint8_t* blob, size_t size, uint8_t* out_sig) const;
#   endif

    inline bool hasMinerSignature() const { return m_hasMinerSignature; }

    uint32_t getNumTransactions() const;

private:
    void copy(const Job &other);
    void move(Job &&other);

    Algorithm m_algorithm;
    bool m_nicehash     = false;
    bool m_donate       = false;
    Buffer m_seed;
    size_t m_size       = 0;
    String m_clientId;
    String m_extraNonce;
    String m_id;
    String m_poolWallet;
    uint32_t m_backend  = 0;
    uint64_t m_diff     = 0;
    uint64_t m_height   = 0;
    uint64_t m_target   = 0;
    uint8_t m_blob[kMaxBlobSize]{ 0 };
    uint8_t m_index     = 0;

#   ifdef XMRIG_PROXY_PROJECT
    char m_rawBlob[kMaxBlobSize * 2 + 8]{};
    char m_rawTarget[24]{};
    String m_rawSeedHash;
    String m_rawSigKey;

    // Miner signatures
    uint8_t m_spendSecretKey[32]{};
    uint8_t m_viewSecretKey[32]{};
    uint8_t m_spendPublicKey[32]{};
    uint8_t m_viewPublicKey[32]{};
    mutable Buffer m_minerTxPrefix;
    size_t m_minerTxEphPubKeyOffset = 0;
    size_t m_minerTxPubKeyOffset = 0;
    size_t m_minerTxExtraNonceOffset = 0;
    size_t m_minerTxExtraNonceSize = 0;
    Buffer m_minerTxMerkleTreeBranch;
    bool m_hasViewTag = false;
#   else
    // Miner signatures
    uint8_t m_ephPublicKey[32]{};
    uint8_t m_ephSecretKey[32]{};
#   endif

    bool m_hasMinerSignature = false;
};


} /* namespace xmrig */


#endif /* XMRIG_JOB_H */
