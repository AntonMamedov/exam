#include <gtest/gtest.h>

#include "bitset.hpp"

using namespace arithmetical::details;

TEST(BitsetTest, DirectCodeTest) {
    {
        DirectBitset sBitset(10, 8);
        ASSERT_EQ("000001010", sBitset.toString());
    }
    {
        DirectBitset sBitset(-10, 8);
        ASSERT_EQ("100001010", sBitset.toString());
    }
}

TEST(BitsetTest, ReverseCodeTest) {
    {
        ReverseBitset sBitset(10, 8);
        ASSERT_EQ("000001010", sBitset.toString());
    }
    {
        ReverseBitset sBitset(-10, 8);
        ASSERT_EQ("111110101", sBitset.toString());
    }
}

TEST(BitsetTest, AdditipnalCodeTest) {
    {
        AdditionalBitset sBitset(10, 8);
        ASSERT_EQ("000001010", sBitset.toString());
    }
    {
        AdditionalBitset sBitset(-10, 8);
        ASSERT_EQ("111110110", sBitset.toString());
    }
}

TEST(BitsetTest, InvertTest) {
    {
        DirectBitset sBitset(10, 8);
        auto sInvertBitset = ~sBitset;
        ASSERT_EQ("111110101", sInvertBitset.toString());
    }
    {
        ReverseBitset sBitset(10, 8);
        auto sInvertBitset = ~sBitset;
        ASSERT_EQ("111110101", sInvertBitset.toString());
    }

    {
        ReverseBitset sBitset(10, 8);
        auto sInvertBitset = ~sBitset;
        ASSERT_EQ("111110101", sInvertBitset.toString());
    }
}

TEST(BitsetTest, InvertSignTest) {
    {
        DirectBitset sBitset(-10, 8);
        auto sInvertBitset = -sBitset;
        ASSERT_EQ("000001010", sInvertBitset.toString());
        ASSERT_EQ(10, static_cast<int>(sInvertBitset));
        sInvertBitset = -sInvertBitset;
        ASSERT_EQ("100001010", sInvertBitset.toString());
        ASSERT_EQ(-10, static_cast<int>(sInvertBitset));
    }
    {
        ReverseBitset sBitset(-10, 8);
        auto sInvertBitset = -sBitset;
        ASSERT_EQ("000001010", sInvertBitset.toString());
        ASSERT_EQ(10, static_cast<int>(sInvertBitset));
        sInvertBitset = -sInvertBitset;
        ASSERT_EQ("111110101", sInvertBitset.toString());
        ASSERT_EQ(-10, static_cast<int>(sInvertBitset));
    }

    {
        AdditionalBitset sBitset(-10, 8);
        auto sInvertBitset = -sBitset;
        ASSERT_EQ("000001010", sInvertBitset.toString());
        ASSERT_EQ(10, static_cast<int>(sInvertBitset));
        sInvertBitset = -sInvertBitset;
        ASSERT_EQ("111110110", sInvertBitset.toString());
        ASSERT_EQ(-10, static_cast<int>(sInvertBitset));
    }
}

TEST(BitsetTest, LeftShiftTest) {
    {
        DirectBitset sBitset(10);
        sBitset <<= 1;
        ASSERT_EQ("000010100", sBitset.toString());
        sBitset <<= 2;
        ASSERT_EQ("001010000", sBitset.toString());
        sBitset <<= 3;
        ASSERT_EQ("010000000", sBitset.toString());
        sBitset <<= 2;
        ASSERT_EQ("000000000", sBitset.toString());
        sBitset = DirectBitset(-10);
        sBitset <<= 1;
        ASSERT_EQ("100010100", sBitset.toString());
        sBitset <<= 2;
        ASSERT_EQ("101010000", sBitset.toString());
        sBitset <<= 3;
        ASSERT_EQ("110000000", sBitset.toString());
        sBitset <<= 2;
        ASSERT_EQ("100000000", sBitset.toString());
    }

    {
        ReverseBitset sBitset(10);
        sBitset <<= 1;
        ASSERT_EQ("000010100", sBitset.toString());
        sBitset <<= 2;
        ASSERT_EQ("001010000", sBitset.toString());
        sBitset <<= 3;
        ASSERT_EQ("010000000", sBitset.toString());
        sBitset <<= 2;
        ASSERT_EQ("000000000", sBitset.toString());
        sBitset = ReverseBitset(-10);
        sBitset <<= 1;
        ASSERT_EQ("111101011", sBitset.toString());
        sBitset <<= 2;
        ASSERT_EQ("110101111", sBitset.toString());
        sBitset <<= 3;
        ASSERT_EQ("101111111", sBitset.toString());
        sBitset <<= 2;
        ASSERT_EQ("111111111", sBitset.toString());
    }

    {
        AdditionalBitset sBitset(10);
        sBitset <<= 1;
        ASSERT_EQ("000010100", sBitset.toString());
        sBitset <<= 2;
        ASSERT_EQ("001010000", sBitset.toString());
        sBitset <<= 3;
        ASSERT_EQ("010000000", sBitset.toString());
        sBitset <<= 2;
        ASSERT_EQ("000000000", sBitset.toString());
        sBitset = AdditionalBitset(-10);
        sBitset <<= 1;
        ASSERT_EQ("111101100", sBitset.toString());
        sBitset <<= 2;
        ASSERT_EQ("110110000", sBitset.toString());
        sBitset <<= 3;
        ASSERT_EQ("110000000", sBitset.toString());
        sBitset <<= 1;
        ASSERT_EQ("000000000", sBitset.toString());
    }
}

TEST(BitsetTest, RightShiftTest) {
    {
        DirectBitset sBitset(10);
        sBitset >>= 1;
        ASSERT_EQ("000000101", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("000000001", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("000000000", sBitset.toString());
        sBitset = DirectBitset(-10);
        sBitset >>= 1;
        ASSERT_EQ("100000101", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("100000001", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("100000000", sBitset.toString());
    }

    {
        ReverseBitset sBitset(10);
        sBitset >>= 1;
        ASSERT_EQ("000000101", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("000000001", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("000000000", sBitset.toString());
        sBitset = ReverseBitset(-10);
        sBitset >>= 1;
        ASSERT_EQ("111111010", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("111111110", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("111111111", sBitset.toString());
    }

    {
        AdditionalBitset sBitset(10);
        sBitset >>= 1;
        ASSERT_EQ("000000101", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("000000001", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("000000000", sBitset.toString());
        sBitset = AdditionalBitset(-10);
        sBitset >>= 1;
        ASSERT_EQ("111111011", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("111111110", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("111111111", sBitset.toString());
        sBitset >>= 2;
        ASSERT_EQ("111111111", sBitset.toString());
    }
}

TEST(BitsetTest, toIntAdditionalTest) {
    {
        AdditionalBitset sBitset(10);
        sBitset <<= 1;
        ASSERT_EQ(20, static_cast<int>(sBitset));
        sBitset <<= 60;
        ASSERT_EQ(0, static_cast<int>(sBitset));

        sBitset = AdditionalBitset(10);
        sBitset >>= 1;
        ASSERT_EQ(5, static_cast<int>(sBitset));
        sBitset >>= 60;
        ASSERT_EQ(0, static_cast<int>(sBitset));

    }
    {
        AdditionalBitset sBitset(-10);
        sBitset <<= 1;
        ASSERT_EQ(-20, static_cast<int>(sBitset));
        sBitset <<= 60;
        ASSERT_EQ(0, static_cast<int>(sBitset));

        sBitset = AdditionalBitset(-10);
        sBitset >>= 1;
        ASSERT_EQ(-5, static_cast<int>(sBitset));
        sBitset >>= 60;
        ASSERT_EQ(-1, static_cast<int>(sBitset));
    }
}

TEST(BitsetTest, toIntReverceTest) {
    {
        ReverseBitset sBitset(10);
        sBitset <<= 1;
        ASSERT_EQ(20, static_cast<int>(sBitset));
        sBitset <<= 60;
        ASSERT_EQ(0, static_cast<int>(sBitset));

        sBitset = ReverseBitset (10);
        sBitset >>= 1;
        ASSERT_EQ(5, static_cast<int>(sBitset));
        sBitset >>= 60;
        ASSERT_EQ(0, static_cast<int>(sBitset));

    }
    {
        ReverseBitset sBitset(-10);
        sBitset <<= 1;
        ASSERT_EQ(-20, static_cast<int>(sBitset));
        sBitset <<= 60;
        ASSERT_EQ(0, static_cast<int>(sBitset));

        sBitset = ReverseBitset (-10);
        sBitset >>= 1;
        ASSERT_EQ(-5, static_cast<int>(sBitset));
        sBitset >>= 60;
        ASSERT_EQ(0, static_cast<int>(sBitset));
    }
}