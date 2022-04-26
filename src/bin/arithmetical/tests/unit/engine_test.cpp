#include <gtest/gtest.h>
#include <iostream>

#include "bitset.hpp"
#include "engine.hpp"

using namespace arithmetical;
using namespace details;
using namespace engine;

namespace {

    void printMulResult(const Multiplier::Result & aRes) {
        std::cout << aRes.m_Val1 << std::endl;
        std::cout << aRes.m_Val2 << std::endl;
        std::cout << "---------" << std::endl;
        for (const auto & sExp : aRes.m_IntermediateExps) {
            std::cout << sExp.m_Val1 << std::endl;
            std::cout << sExp.m_Val2 << std::endl;
            std::cout << "---------" << std::endl;
        }
        std::cout << aRes.m_Result << std::endl;
    }

}

TEST(EngineTst, TestMulWithCorrectionStepAdditionalCode) {
    AdditionalBitset sMul(9, 8);
    AdditionalBitset sFactor(-12, 8);

    Multiplier sMultiplier(5);
    auto sResult = sMultiplier.mulWithCurrentStep(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "110010100");

    sMul = AdditionalBitset(-9, 8);
    sFactor = AdditionalBitset(12, 8);

    sResult = sMultiplier.mulWithCurrentStep(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "110010100");

    sMul = AdditionalBitset(-9, 8);
    sFactor = AdditionalBitset(-12, 8);

    sResult = sMultiplier.mulWithCurrentStep(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "001101100");

    sMul = AdditionalBitset(9, 8);
    sFactor = AdditionalBitset(12, 8);

    sResult = sMultiplier.mulWithCurrentStep(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "001101100");
}

TEST(EngineTst, TestMulWithCorrectionStepReverseCode) {
    ReverseBitset sMul(9, 8);
    ReverseBitset sFactor(-12, 8);

    Multiplier sMultiplier(5);

    auto sResult = sMultiplier.mulWithCurrentStep(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "110010011");

    sMul = ReverseBitset(-9, 8);
    sFactor = ReverseBitset(12, 8);

    sResult = sMultiplier.mulWithCurrentStep(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "110010011");

    sMul = ReverseBitset(9, 8);
    sFactor = ReverseBitset(12, 8);

    sResult = sMultiplier.mulWithCurrentStep(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "001101100");

    sMul = ReverseBitset(-9, 8);
    sFactor = ReverseBitset(-12, 8);

    sResult = sMultiplier.mulWithCurrentStep(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "001101100");
}

TEST(EngineTst, TestMulWithInvertNumberBits) {
    ReverseBitset sMul(9, 8);
    ReverseBitset sFactor(-12, 8);

    Multiplier sMultiplier(5);

    auto sResult = sMultiplier.mulWithInvertNumberBits(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "110010011");

    sMul = ReverseBitset(-9, 8);
    sFactor = ReverseBitset(12, 8);

    sResult = sMultiplier.mulWithInvertNumberBits(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "110010011");

    sMul = ReverseBitset(9, 8);
    sFactor = ReverseBitset(12, 8);

    sResult = sMultiplier.mulWithInvertNumberBits(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "001101100");

    sMul = ReverseBitset(-9, 8);
    sFactor = ReverseBitset(-12, 8);

    sResult = sMultiplier.mulWithInvertNumberBits(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "001101100");
}

TEST(EngineTst, TestMulWithAnalysisAdjacentBits) {
    AdditionalBitset sMul(-9, 8);
    AdditionalBitset sFactor(-13, 8);

    Multiplier sMultiplier(5);
    auto sResult = sMultiplier.mulWithAnalysisAdjacentBits(sMul, sFactor);
    ASSERT_EQ(sResult.m_Result, "001110101");

    sMul = AdditionalBitset(9, 8);
    sFactor = AdditionalBitset(13, 8);

    sResult = sMultiplier.mulWithAnalysisAdjacentBits(sMul, sFactor);
    ASSERT_EQ(sResult.m_Result, "001110101");

    sMul = AdditionalBitset(-9, 8);
    sFactor = AdditionalBitset(13, 8);

    sResult = sMultiplier.mulWithAnalysisAdjacentBits(sMul, sFactor);
    ASSERT_EQ(sResult.m_Result, "110001011");

    sMul = AdditionalBitset(9, 8);
    sFactor = AdditionalBitset(-13, 8);

    sResult = sMultiplier.mulWithAnalysisAdjacentBits(sMul, sFactor);
    ASSERT_EQ(sResult.m_Result, "110001011");
}