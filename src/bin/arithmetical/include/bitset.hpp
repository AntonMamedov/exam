#pragma once

#include <bitset>

namespace arithmetical {

    constexpr const size_t BITSET_SIZE = 16;
    using bits_t = std::bitset<BITSET_SIZE>;
    class Bitset : public bits_t {
    public:
        Bitset(bits_t aBitset, size_t aBits, size_t aBitsGrid);
        Bitset(int aVal, size_t aBits, size_t aBitsGrid);
        Bitset(const Bitset&) = default;
        Bitset(Bitset&&) = default;

        std::string toString() const;

        Bitset& operator=(const Bitset&) = default;
        Bitset& operator=(Bitset&&) = default;
        Bitset &operator<<=(size_t aShift);
        Bitset &operator>>=(size_t aShift);
        Bitset operator ~() const;
        friend std::ostream& operator << (std::ostream &out, const Bitset& sBitset);
    private:
        size_t m_Bits;
        size_t m_BitsGrid;
        bits_t m_Cleaner;
    };

}