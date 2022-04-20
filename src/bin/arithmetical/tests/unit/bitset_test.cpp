#include <gtest/gtest.h>

#include "bitset.hpp"

using namespace arithmetical;

TEST(BitsetTest, DirectCodeTest) {
    Bitset sBitset1(3, 5, 8, arithmetical::Code::DIRECT);
    ASSERT_EQ("00011", sBitset1.to_string());
    Bitset sBitset2(-3, 5, 8, arithmetical::Code::DIRECT);
    ASSERT_EQ("10011", sBitset2.to_string());
}

TEST(BitsetTest, ReverseCodeTest) {
    Bitset sBitset1(3, 5, 8, arithmetical::Code::REVERSE);
    ASSERT_EQ("00011", sBitset1.to_string());
    Bitset sBitset2(-3, 5, 8, arithmetical::Code::REVERSE);
    ASSERT_EQ("11100", sBitset2.to_string());
}

TEST(BitsetTest, AdditipnalCodeTest) {
    Bitset sBitset1(3, 5, 8, arithmetical::Code::ADDITIONAL);
    ASSERT_EQ("00011", sBitset1.to_string());
    Bitset sBitset2(-3, 5, 8, arithmetical::Code::ADDITIONAL);
    ASSERT_EQ("11101", sBitset2.to_string());
}

TEST(BitsetTest, InvertTest) {
    ASSERT_EQ("11100", (~Bitset(0b00011, 5, 8)).to_string());
    ASSERT_EQ("011001", (~Bitset(0b100110, 6, 8)).to_string());

    ASSERT_EQ("11100", (~Bitset(0b00011, 5, 8, arithmetical::Code::REVERSE)).to_string());
    ASSERT_EQ("011001", (~Bitset(0b100110, 6, 8, arithmetical::Code::REVERSE)).to_string());

    ASSERT_EQ("11100", (~Bitset(0b00011, 5, 8, arithmetical::Code::ADDITIONAL)).to_string());
    ASSERT_EQ("011001", (~Bitset(0b100110, 6, 8, arithmetical::Code::ADDITIONAL)).to_string());
}

TEST(BitsetTest, InvertSignPositiveToNegativeTest) {
    Bitset sBitset1(0b00011, 5, 8, arithmetical::Code::DIRECT);
    ASSERT_EQ("00011", sBitset1.to_string());
    ASSERT_EQ("10011", sBitset1.invertSign().to_string());

    Bitset sBitset2(0b00011, 5, 8, arithmetical::Code::REVERSE);
    ASSERT_EQ("00011", sBitset2.to_string());
    ASSERT_EQ("11100", sBitset2.invertSign().to_string());

    Bitset sBitset3(0b00011, 5, 8, arithmetical::Code::ADDITIONAL);
    ASSERT_EQ("00011", sBitset3.to_string());
    ASSERT_EQ("11101", sBitset3.invertSign().to_string());
}

TEST(BitsetTest, InvertSignNegativeToPositiveTest) {
    Bitset sBitset1(0b10011, 5, 8, arithmetical::Code::DIRECT);
    ASSERT_EQ("10011", sBitset1.to_string());
    ASSERT_EQ("00011", sBitset1.invertSign().to_string());

    Bitset sBitset2(0b11100, 5, 8, arithmetical::Code::REVERSE);
    ASSERT_EQ("11100", sBitset2.to_string());
    ASSERT_EQ("00011", sBitset2.invertSign().to_string());

    Bitset sBitset3(0b11101, 5, 8, arithmetical::Code::ADDITIONAL);
    ASSERT_EQ("11101", sBitset3.to_string());
    ASSERT_EQ("00011", sBitset3.invertSign().to_string());

}

TEST(BitsetTest, LeftShiftTest) {
    ASSERT_EQ("000110", (Bitset(0b00011, 5, 8) <<= 1).to_string());
}

TEST(BitsetTest, RightShiftTest) {
    ASSERT_EQ("0001", (Bitset(0b00011, 5, 8) >>= 1).to_string());
}