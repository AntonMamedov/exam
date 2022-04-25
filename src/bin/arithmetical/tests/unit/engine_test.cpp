#include <gtest/gtest.h>
#include <iostream>

#include "bitset.hpp"
#include "engine.hpp"

using namespace arithmetical;
using namespace details;
using namespace engine;

namespace {

    void printMulResult(const Multiplier::Result& aRes){
        std::cout << aRes.m_Val1 << std::endl;
        std::cout << aRes.m_Val2 << std::endl;
        std::cout << "---------"<< std::endl;
        for (const auto& sExp : aRes.m_IntermediateExps) {
            std::cout << sExp.m_Val1 << std::endl;
            std::cout << sExp.m_Val2 << std::endl;
            std::cout << "---------"<< std::endl;
        }
        std::cout << aRes.m_Result << std::endl;
    }
}

TEST(EngineTst, TestMulWithCorrectionStepAdditionalCode) {
    iBitset::ptr sMul = std::make_unique<AdditionalBitset>(9, 8);
    iBitset::ptr sFactor = std::make_unique<AdditionalBitset>(-12, 8);

    Multiplier sMultiplier(5);
    auto sResult = sMultiplier.mulWithCurrentStep(sMul, sFactor);

    ASSERT_EQ(sResult.m_Result, "110010100");
}

TEST(EngineTst, TestMulWithCorrectionStepReverseCode) {
    iBitset::ptr sMul = std::make_unique<ReverseBitset>(9, 8);
    iBitset::ptr sFactor = std::make_unique<ReverseBitset>(-12, 8);

    Multiplier sMultiplier(5);

    auto sResult = sMultiplier.mulWithCurrentStep(sMul, sFactor);


    ASSERT_EQ(sResult.m_Result, "110010011");
}