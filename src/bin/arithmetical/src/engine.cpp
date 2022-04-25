#include "engine.hpp"

#include "bitset.hpp"

using namespace arithmetical;
using namespace details;
using namespace engine;

namespace {
    constexpr const auto DIFFERENT_CODES_ERRORS = "added values cannot be represented in different codes\n";
    constexpr const auto CODE_NOT_SUPPORT = "this operation does not support this code\n";

    std::string bitset2string(const bits_t& aBitset) {
        std::string sRes;
        to_string(aBitset, sRes);
        return sRes;
    }
}


Multiplier::Result
Multiplier::mulWithCurrentStep(const iBitset::ptr & aMul, const iBitset::ptr & aFactor) {
    if (aMul->code() != aFactor->code()) {
        throw std::logic_error(DIFFERENT_CODES_ERRORS);
    }

    if (aMul->code() == details::Code::DIRECT) {
        throw std::logic_error(CODE_NOT_SUPPORT);
    }

    auto sCode = aMul->code();

    iBitset::ptr sMul = aMul->copy();
    iBitset::ptr sFactor = aFactor->copy();

    sMul->leftShift(sMul->size() - (m_NumberSize));
    sFactor->leftShift(sMul->size() - (m_NumberSize));

    std::vector<bits_t> sBitsets;

    // делаем корректирующий шаг если это дополнительный код
    if (sCode == Code::ADDITIONAL) {
        if (!sFactor->isPositive())
            sBitsets.emplace_back(sMul->invertSign()->bitset());
        else
            sBitsets.emplace_back(sMul->size(), 0);
    }

    for (size_t i = 1; i < m_NumberSize; i++) {
        if (!sFactor->invertGet(i)) {
            sBitsets.emplace_back(sMul->size(), 0);
            continue;
        }
        auto sCurrent = sMul->copy();
        sCurrent->rightShift(i);
        std::string t4;
        to_string(sCurrent->bitset(), t4);
        sBitsets.emplace_back(sCurrent->bitset());
    }

    std::vector<Expression> sExps;
    bits_t sPartialSum(sMul->size(), 0);
    for (auto & sBitset : sBitsets) {
        Expression sExp{
                .m_Val1 = bitset2string(sPartialSum),
                .m_Val2 = bitset2string(sBitset),
                .m_Action = Action::ADD,
                .m_IsCorrectionStep = false
        };
        sPartialSum = addBitsets(sPartialSum, sBitset);
        sExps.emplace_back(std::move(sExp));
    }

    if (sCode == Code::ADDITIONAL) {
        sExps[0].m_IsCorrectionStep = true;
    } else if (sCode == Code::REVERSE && !aFactor->isPositive()){
        auto sCorrectionStepBitset = sMul->invertSign()->bitset();
        Expression sExp{
                .m_Val1 = bitset2string(sPartialSum),
                .m_Val2 = bitset2string(sCorrectionStepBitset),
                .m_Action = Action::ADD,
                .m_IsCorrectionStep = true
        };
        sPartialSum = addBitsets(sPartialSum, sCorrectionStepBitset);
        sExps.emplace_back(std::move(sExp));

        auto sBitset = sMul->copy();
        sBitset->rightShift(m_NumberSize - 1);
        sCorrectionStepBitset = sBitset->bitset();
        sExp = {
                .m_Val1 = bitset2string(sPartialSum),
                .m_Val2 = bitset2string(sCorrectionStepBitset),
                .m_Action = Action::ADD,
                .m_IsCorrectionStep = true
        };
        sPartialSum = addBitsets(sPartialSum, sCorrectionStepBitset);
        sExps.emplace_back(std::move(sExp));
    }

    return Result{
            .m_Code = sCode,
            .m_Val1 = bitset2string(  aMul->bitset()).substr(aMul->size() - m_NumberSize),
            .m_Val2 = bitset2string(aFactor->bitset()).substr(aFactor->size() - m_NumberSize),
            .m_Result = bitset2string(sPartialSum),
            .m_IntermediateExps = std::move(sExps)
    };
}

