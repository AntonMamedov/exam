#pragma once

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <memory>


namespace arithmetical::details {
    using bits_t = boost::dynamic_bitset<uint16_t>;
    using bit_t = bool;

    constexpr const auto BIT_MASK = 0xFFFFFFFFFFFFFFFFUL;
    constexpr const auto DEFAULT_GRID_SIZE = 8UL;

    enum class Code {
        DIRECT = 1,
        REVERSE,
        ADDITIONAL,
    };

    using bit_t = bool;

    constexpr const bit_t OFF = false;
    constexpr const bit_t ON = true;

    template<Code code>
    class Bitset {
    public:
        class ReverseReference {
        public:
            explicit ReverseReference(Bitset<code> & aRef) : m_Parent{aRef} {
            }

            bit_t operator[](size_t aPos) const {
                if (aPos == 0) {
                    return m_Parent.m_SignBit;
                }
                return m_Parent.m_Data[m_Parent.m_GridSize - aPos];
            };
        private:
            Bitset<code> & m_Parent;
        };

        explicit Bitset(int aData, size_t aGridSize = DEFAULT_GRID_SIZE);

        Bitset operator-() const;
        explicit operator int() const;

        Bitset & operator<<=(size_t aPos);

        Bitset & operator>>=(size_t aPos) {
            m_Data >>= aPos;
            if (m_SignBit) {
                bits_t sMask(m_GridSize, BIT_MASK);
                sMask <<= static_cast<int>(m_GridSize) - static_cast<int>(aPos) > 0 ? m_GridSize - aPos : 0;
                m_Data |= sMask;
            }
            return *this;
        };

        bit_t operator[](size_t aPos) const {
            if (aPos == m_GridSize) {
                return m_SignBit;
            }
            return m_Data[aPos];
        };

        std::string toString() const {
            std::string sSign = !m_SignBit ? "0" : "1";
            std::string sData;
            to_string(m_Data, sData);
            return sSign + sData;
        };


        bits_t bitset() const {
            auto sBitset = m_Data;
            sBitset.push_back(m_SignBit);
            return sBitset;
        }

        size_t size() const {
            return m_GridSize + 1;
        }

        Bitset operator~() const {
            auto sBitset = *this;
            sBitset.m_Data = ~sBitset.m_Data;
            sBitset.m_SignBit = !sBitset.m_SignBit;
            return sBitset;
        };


        bool operator<(int aNum) const {
            return static_cast<int>(*this) < aNum;
        }

        bool operator<=(int aNum) const {
            return static_cast<int>(*this) <= aNum;
        }

        bool operator>(int aNum) const {
            return static_cast<int>(*this) > aNum;
        }

        bool operator>=(int aNum) const {
            return static_cast<int>(*this) >= aNum;
        }

        bool operator==(int aNum) const {
            return static_cast<int>(*this) == aNum;
        }

        bool operator!=(int aNum) const {
            return static_cast<int>(*this) != aNum;
        }

        ReverseReference reverse() {
            return {*this};
        }
    private:
        size_t m_GridSize;
        bit_t m_SignBit;
        bits_t m_Data;
    };

    template<Code code>
    Bitset<code>::Bitset(int aData, size_t aGridSize) :m_GridSize(aGridSize), m_SignBit(OFF) {
        if constexpr(code == Code::DIRECT) {
            if (aData < 0) {
                m_SignBit = ON;
                aData = -aData;
            }
        } else if (code == Code::REVERSE) {
            if (aData < 0) {
                aData -= 1;
                m_SignBit = ON;
            }
        } else {
            if (aData < 0)
                m_SignBit = ON;
        }
        m_Data = bits_t(aGridSize, aData);
    }

    using DirectBitset = Bitset<Code::DIRECT>;
    using ReverseBitset = Bitset<Code::REVERSE>;
    using AdditionalBitset = Bitset<Code::ADDITIONAL>;

    bits_t operator + (const bits_t& aLeft, const bits_t& aRight);
}