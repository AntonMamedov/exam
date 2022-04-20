#pragma once

#include <bitset>

namespace arithmetical {

    enum class Code {
        DIRECT = 1,
        REVERSE,
        ADDITIONAL
    };

    constexpr const size_t BITSET_SIZE = 16;

    using bits_t = std::bitset<BITSET_SIZE>;
    using bit = bool;

    class Bitset {
    public:
        Bitset(bits_t aBitset, size_t aBits, size_t aBitsGrid, Code aCode = Code::DIRECT);
        Bitset(int aVal, size_t aBits, size_t aBitsGrid, Code aCode = Code::DIRECT);
        Bitset(const Bitset &) = default;
        Bitset(Bitset &&) = default;

        std::string to_string() const;
        Bitset invertSign() const;

        Bitset &operator=(const Bitset &) = default;
        Bitset &operator=(Bitset &&) = default;
        Bitset &operator<<=(size_t aShift);
        Bitset &operator>>=(size_t aShift);
        Bitset operator~() const;
        bits_t::reference operator[](size_t aPos);
        bool operator[](size_t aPos) const;
        friend std::ostream &operator<<(std::ostream &out, const Bitset &sBitset);

    private:
        void updateCleaner();
    private:
        size_t m_Bits;
        size_t m_BitsGrid;
        Code   m_Code;
        bits_t m_Data;
        bits_t m_Cleaner;
    };

}