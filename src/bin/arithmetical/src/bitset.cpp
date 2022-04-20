#include "bitset.hpp"

using namespace arithmetical;

Bitset::Bitset(bits_t aBitset, size_t aBits, size_t aBitsGrid, Code aCode)
        : m_Bits(aBits), m_BitsGrid(aBitsGrid), m_Code(aCode), m_Data(aBitset) {
    updateCleaner();
}

Bitset::Bitset(int aVal, size_t aBits, size_t aBitsGrid, Code aCode)
        : m_Bits(aBits), m_BitsGrid(aBitsGrid), m_Code(aCode) {
    switch (aCode) {
        case Code::DIRECT:
            if (aVal >= 0) {
                m_Data = bits_t(aVal);
                break;
            }
            m_Data = bits_t(-aVal);
            m_Data.set(m_Bits - 1, true);
            break;
        case Code::REVERSE:
            if (aVal < 0)
                aVal -= 1;
            m_Data = bits_t(aVal);
            break;
        case Code::ADDITIONAL:
            m_Data = bits_t(aVal);
            break;
    }
    updateCleaner();
}

Bitset &Bitset::operator<<=(size_t aShift) {
    m_Data <<= aShift;
    m_Bits += aShift;
    updateCleaner();
    return *this;
}

Bitset &Bitset::operator>>=(size_t aShift) {
    m_Data >>= aShift;
    m_Bits -= std::min(aShift, m_Bits);
    updateCleaner();
    return *this;
}

Bitset Bitset::operator~() const {
    bits_t sTmp(~m_Data);
    sTmp &= m_Cleaner;
    return {sTmp, m_Bits, m_BitsGrid};
}

std::string Bitset::to_string() const {
    auto sStr = m_Data.to_string();
    return {sStr, sStr.size() - m_Bits, m_Bits};
}

std::ostream &arithmetical::operator<<(std::ostream &out, const Bitset &sBitset) {
    out << sBitset.to_string();
    return out;
}

bits_t::reference Bitset::operator[](size_t aPos) {
    return m_Data[aPos];
}

bool Bitset::operator[](size_t aPos) const {
    return m_Data[aPos];
}

void Bitset::updateCleaner() {
    m_Cleaner = 0xFFFF;
    m_Cleaner >>= m_Bits;
}

Bitset Bitset::invertSign() const {
    auto sTmp = *this;
    switch (m_Code) {
        case Code::DIRECT:
            sTmp.m_Data[m_Bits - 1] =  !sTmp.m_Data[m_Bits - 1];
            break;
        case Code::REVERSE:
            sTmp = ~sTmp;
            break;
        case Code::ADDITIONAL:
            sTmp = Bitset((~sTmp).m_Data.to_ulong() + 1, m_Bits, m_BitsGrid);
            break;
    }
    return sTmp;
}

