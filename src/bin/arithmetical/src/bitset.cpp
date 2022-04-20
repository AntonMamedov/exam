#include "bitset.hpp"

using namespace arithmetical;

Bitset::Bitset(bits_t aBitset, size_t aBits, size_t aBitsGrid)
        : std::bitset<BITSET_SIZE>(aBitset), m_Bits(aBits), m_BitsGrid(aBitsGrid) {
    m_Cleaner = 0xFFFF;
    m_Cleaner >>= BITSET_SIZE - m_Bits;
}

Bitset::Bitset(int aVal, size_t aBits, size_t aBitsGrid)
        : Bitset(bits_t(aVal), aBits, aBitsGrid) {}

Bitset &Bitset::operator<<=(size_t aShift) {
    bits_t sTmp = static_cast<bits_t>(*this);
    sTmp <<= aShift;
    *this = Bitset(sTmp, m_Bits + aShift, m_BitsGrid);
    return *this;
}

Bitset &Bitset::operator>>=(size_t aShift) {
    bits_t sTmp = static_cast<bits_t>(*this);
    sTmp >>= aShift;

    *this = Bitset(sTmp, std::max<int>(static_cast<int>(m_Bits) - static_cast<int>(aShift), 0), m_BitsGrid);
    return *this;
}

Bitset Bitset::operator~() const {
    bits_t sTmp = ~(static_cast<bits_t>(*this));
    sTmp &= m_Cleaner;
    return {sTmp, m_Bits, m_BitsGrid};
}

std::string Bitset::toString() const {
    return {to_string(), size() - m_Bits, m_Bits};
}

std::ostream &arithmetical::operator<<(std::ostream &out, const Bitset &sBitset) {
    out << sBitset.toString();
    return out;
}

