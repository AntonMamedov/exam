#pragma once

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <memory>


namespace arithmetical::details {
    using bits_t = boost::dynamic_bitset<uint16_t>;
    using bit_t = bool;

    constexpr const size_t DEFAULT_BITS_GRID = 8;

    enum class Code {
        DIRECT = 1,
        REVERSE,
        ADDITIONAL,
    };

    using bit_t = bool;

    constexpr const bit_t OFF = false;
    constexpr const bit_t ON = true;

    class iBitset {
    public:
        using ptr = std::unique_ptr<iBitset>;

        virtual void set(size_t aPos, bit_t aBit) = 0;
        virtual void leftShift(size_t aPos) = 0;
        virtual void rightShift(size_t aPos) = 0;
        virtual bit_t get(size_t aPos) const = 0;
        virtual bit_t invertGet(size_t aPos) const = 0;
        virtual ptr invert() const = 0;
        virtual int toInt() const = 0;
        virtual bool isPositive() const = 0;
        virtual Code code() const = 0;
        virtual std::string toString() const = 0;
        virtual ptr invertSign() const = 0;
        virtual bits_t bitset() const = 0;
        virtual ptr copy() const = 0;
        virtual size_t size() const = 0;
        virtual ~iBitset() = default;
    };

    class BitsetImpl : public iBitset {
    public:
        void set(size_t aPos, bit_t aBit) override {
            m_Data[aPos] = aBit;
        };

        bit_t get(size_t aPos) const override {
            if (aPos == m_GridSize) {
                return m_SignBit;
            }
            return m_Data[aPos];
        };

        bit_t invertGet(size_t aPos) const override {
            if (aPos == 0) {
                return m_SignBit;
            }
            return m_Data[m_GridSize - aPos];
        }

        bool isPositive() const override {
            return !m_SignBit;
        };

        std::string toString() const override {
            std::string sSign = !m_SignBit ? "0" : "1";
            std::string sData;
            to_string(m_Data, sData);
            return sSign + sData;
        };

        bits_t bitset() const override{
            auto sBitset = m_Data;
            sBitset.push_back(m_SignBit);
            return sBitset;
        }

        size_t size() const override{
            return m_GridSize + 1;
        }
    protected:
        size_t m_GridSize;
        bit_t m_SignBit;
        bits_t m_Data;
    };

    class DirectBitset final : public BitsetImpl {
    public:
        DirectBitset(int aData, size_t aGridSize = DEFAULT_BITS_GRID);
        void leftShift(size_t aPos) override;
        void rightShift(size_t aPos) override;
        ptr invert() const override;
        int toInt() const override;
        ptr invertSign() const override;
        ptr copy() const override;
        Code code() const override {
            return Code::DIRECT;
        };
    };

    class ReverseBitset final : public BitsetImpl {
    public:
        ReverseBitset(int aData, size_t aGridSize = DEFAULT_BITS_GRID);

        void leftShift(size_t aPos) override;
        void rightShift(size_t aPos) override;
        ptr invert() const override;
        int toInt() const override;
        ptr invertSign() const override;
        ptr copy() const override;
        Code code() const override {
            return Code::REVERSE;
        };
    };

    class AdditionalBitset final : public BitsetImpl {
    public:
        AdditionalBitset(int aData, size_t aGridSize = DEFAULT_BITS_GRID);

        void leftShift(size_t aPos) override;
        void rightShift(size_t aPos) override;
        ptr invert() const override;
        int toInt() const override;
        ptr invertSign() const override;
        ptr copy() const override;
        Code code() const override {
            return Code::ADDITIONAL;
        };
    };

    bits_t addBitsets(const bits_t& aLeft, const bits_t& aRight);
}