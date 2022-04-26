#include "bitset.hpp"

using namespace arithmetical::details;

template<>
Bitset<Code::DIRECT> & Bitset<Code::DIRECT>::operator<<=(size_t aPos) {
    m_Data <<= aPos;
    return *this;
}

template<>
Bitset<Code::REVERSE> & Bitset<Code::REVERSE>::operator<<=(size_t aPos) {
    m_Data <<= aPos;
    if (m_SignBit) {
        bits_t sMask(m_GridSize, BIT_MASK);
        sMask >>= static_cast<int>(m_GridSize) - static_cast<int>(aPos) > 0 ? m_GridSize - aPos : 0;
        m_Data |= sMask;
    }
    return *this;
}

template<>
Bitset<Code::ADDITIONAL> & Bitset<Code::ADDITIONAL>::operator<<=(size_t aPos) {
    m_Data <<= aPos;
    if (m_Data.none()) {
        m_SignBit = OFF;
    }
    return *this;
}

template<>
Bitset<Code::DIRECT> & Bitset<Code::DIRECT>::operator>>=(size_t aPos) {
    m_Data >>= aPos;
    return *this;
}

template<>
Bitset<Code::DIRECT> Bitset<Code::DIRECT>::operator-() const {
    auto sBitset = *this;
    sBitset.m_SignBit = !m_SignBit;
    return sBitset;
}

template<>
Bitset<Code::REVERSE> Bitset<Code::REVERSE>::operator-() const {
    auto sBitset = *this;
    sBitset.m_Data = ~sBitset.m_Data;
    sBitset.m_SignBit = !m_SignBit;
    return sBitset;
}

template<>
Bitset<Code::ADDITIONAL> Bitset<Code::ADDITIONAL>::operator-() const {
    auto sBitset = *this;
    uint64_t sData = sBitset.m_Data.to_ulong();
    sData = ~sData;
    sData += 1;
    sBitset.m_Data = bits_t(m_GridSize, sData);
    sBitset.m_SignBit = !sBitset.m_SignBit;
    return sBitset;
}

template<>
Bitset<Code::DIRECT>::operator int() const {
    bits_t sDataBits = m_Data;
    int sData = static_cast<int>(sDataBits.to_ulong());

    return !m_SignBit ? sData : -sData;
}

template<>
Bitset<Code::REVERSE>::operator int() const {
    if (!m_SignBit) {
        return static_cast<int>(m_Data.to_ulong());
    }
    auto sData = ~m_Data;
    return -static_cast<int>(sData.to_ulong());
}

template<>
Bitset<Code::ADDITIONAL>::operator int() const {
    if (!m_SignBit)
        return static_cast<int>(m_Data.to_ulong());

    return -static_cast<int>((~m_Data).to_ulong() + 1);
}

bits_t arithmetical::details::operator+(const bits_t & aLeft, const bits_t & aRight) {
    uint64_t sRes = aLeft.to_ulong() + aRight.to_ulong();
    size_t sSize = aLeft.size() > aRight.size() ? aLeft.size() : aRight.size();
    uint64_t sLimit = BIT_MASK >> ((sizeof(uint64_t) * 8) - sSize);
    if (sRes > sLimit)
        sSize++;
    return bits_t(sSize, sRes);
}
