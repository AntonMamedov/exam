#include <gtest/gtest.h>
#include <iostream>

#include "bitset.hpp"

using namespace arithmetical;

TEST(BitsetTest, InvertTest) {
    ASSERT_EQ("11100", (~Bitset(0b00011, 5, 8)).toString());
    ASSERT_EQ("011001", (~Bitset(0b100110, 6, 8)).toString());
}

TEST(BitsetTest, LeftShiftTest) {
    ASSERT_EQ("000110", (Bitset(0b00011, 5, 8) <<= 1).toString());
}

TEST(BitsetTest, RightShiftTest) {
    ASSERT_EQ("0001", (Bitset(0b00011, 5, 8) >>= 1).toString());
}