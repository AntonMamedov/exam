#pragma once

#include <memory>
#include <vector>

#include "bitset.hpp"

namespace arithmetical::engine {

    constexpr const auto CODE_NOT_SUPPORT = "this operation does not support this code\n";

    enum class Action {
        NONE = 0,
        ADD,
        SUB,
        MUL,
        DIV
    };

    class Multiplier {
    public:
        enum class Method {
            CORRECTION_STEP,
            ANALYSIS_ADJACENT_BITS,
            INVERT_NUMBER_BITS,
            ON_TWO_FACTOR_BITS,
            FROM_LOW_BITS,
            FROM_HIGH_BITS
        };

        struct Expression {
            std::string m_Val1;
            std::string m_Val2;
            Action m_Action;
            bool m_IsCorrectionStep;
            bool m_IsOverflow;
        };

        struct Result {
            details::Code m_Code;
            Method m_Method;
            std::string m_Val1;
            std::string m_Val2;
            std::string m_Result;
            std::vector<Expression> m_IntermediateExps;
        };

        explicit Multiplier(size_t aNumberSize) : m_NumberSize(aNumberSize) {
        }

        template<details::Code code>
        Multiplier::Result mulWithCurrentStep(const details::Bitset<code> & aMul,
                                              const details::Bitset<code> & aFactor) const {
            throw std::logic_error(CODE_NOT_SUPPORT);
        };

        Multiplier::Result mulOnTwoFactorBits(const details::DirectBitset & aMul,
                                              const details::DirectBitset & aFactor) const;
        Multiplier::Result mulWithAnalysisAdjacentBits(const details::AdditionalBitset & aMul,
                                                       const details::AdditionalBitset & aFactor) const;
        Multiplier::Result mulWithInvertNumberBits(const details::ReverseBitset & aMul,
                                                   const details::ReverseBitset & aFactor) const;
        Multiplier::Result mulFromLowBits(const details::DirectBitset & aMul,
                                          const details::DirectBitset & aFactor) const;
        Multiplier::Result mulFromHighBits(const details::DirectBitset & aMul,
                                           const details::DirectBitset & aFactor) const;

    private:
        size_t m_NumberSize;
    };

    class Adder {
    public:
        template<details::Code code>
        static std::string Add(int aVal1, int aVal2, size_t aGridSize) {
            int sRes = aVal1 + aVal2;
            details::Bitset<code> aResCode (sRes, aGridSize);
            return aResCode.toString();
        }
    };
}