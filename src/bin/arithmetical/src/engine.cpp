#include "engine.hpp"
#include "bitset.hpp"

#include <queue>

using namespace arithmetical;
using namespace details;
using namespace engine;

namespace {
    constexpr const auto DIFFERENT_CODES_ERRORS = "added values cannot be represented in different codes\n";

    std::string bitset2string(const bits_t & aBitset) {
        std::string sRes;
        to_string(aBitset, sRes);
        return sRes;
    }

    bits_t string2bitset(const std::string & aStr) {
        bits_t sRes;
        for (char sBit : aStr) {
            sRes.push_back(sBit == '1');
        }
        return sRes;
    }

    Action bitOrderToAction(bit_t aBi1, bit_t aBit2) {
        if (!aBi1 && aBit2)
            return Action::ADD;
        else if (aBi1 && !aBit2)
            return Action::SUB;
        return Action::NONE;
    }

}

template<>
Multiplier::Result
Multiplier::mulWithCurrentStep(const AdditionalBitset & aMul,
                               const AdditionalBitset & aFactor) const {

    auto sMul = aMul;
    auto sFactor = aFactor;

    sMul <<= sMul.size() - (m_NumberSize);
    sFactor <<= sFactor.size() - (m_NumberSize);

    std::vector<bits_t> sBitsets;
    sBitsets.emplace_back((-sMul).bitset());

    auto sReverseFactor = sFactor.reverse();
    for (size_t i = 1; i < m_NumberSize; i++) {

        if (!sReverseFactor[i]) {
            sBitsets.emplace_back(sMul.size(), 0);
            continue;
        }
        auto sCurrent = sMul;
        sCurrent >>= i;
        sBitsets.emplace_back(sCurrent.bitset());
    }

    std::vector<Expression> sExps;
    bits_t sPartialSum(sMul.size(), 0);
    bool sCorrectionStep = true;
    for (auto & sBitset : sBitsets) {
        auto sAction = Action::ADD;
        if (sCorrectionStep)
            sAction = sFactor.sign() ? Action::SUB : Action::NONE;

        std::string sVal1 = bitset2string(sPartialSum);;
        std::string sVal2 = bitset2string(sBitset);;
        switch (sAction) {
            case Action::SUB:
            case Action::ADD:
                sPartialSum = sPartialSum + sBitset;
                break;
            default:
                break;
        }

        bool sIsOverflow = false;

        if (sPartialSum.size() > sBitset.size()) {
            sIsOverflow = true;
            sPartialSum.pop_back();
        }

        Expression sExp{
                .m_Val1 = sVal1,
                .m_Val2 = sVal2,
                .m_Action = Action::ADD,
                .m_IsCorrectionStep = sCorrectionStep,
                .m_IsOverflow = sIsOverflow
        };

        sExps.emplace_back(std::move(sExp));
        sCorrectionStep = false;
    }

    if (sFactor.sign()) {
        sExps.front().m_Action = Action::SUB;
        sExps.front().m_IsCorrectionStep = true;
    }

    return Result{
            .m_Code = Code::ADDITIONAL,
            .m_Method = Method::CORRECTION_STEP,
            .m_Val1 = bitset2string(aMul.bitset()).substr(aMul.size() - m_NumberSize),
            .m_Val2 = bitset2string(aFactor.bitset()).substr(aFactor.size() - m_NumberSize),
            .m_Result = bitset2string(sPartialSum),
            .m_IntermediateExps = std::move(sExps)
    };
}

template<>
Multiplier::Result
Multiplier::mulWithCurrentStep(const ReverseBitset & aMul,
                               const ReverseBitset & aFactor) const {

    auto sMul = aMul;
    auto sFactor = aFactor;

    sMul <<= sMul.size() - (m_NumberSize);
    sFactor <<= sFactor.size() - (m_NumberSize);

    std::queue<std::pair<bits_t, bool>> sBitsets;

    auto sReverseFactor = sFactor.reverse();
    for (size_t i = 1; i < m_NumberSize; i++) {

        if (!sReverseFactor[i]) {
            sBitsets.push({bits_t{sMul.size(), 0}, false});
            continue;
        }
        auto sCurrent = sMul;
        sCurrent >>= i;
        sBitsets.push({sCurrent.bitset(), false});
    }

    if (sFactor.sign()) {
        auto sBitset = sMul;
        sBitset >>= m_NumberSize - 1;
        sBitsets.push({(-sMul).bitset(), true});
        sBitsets.push({sBitset.bitset(), true});
    }

    std::vector<Expression> sExps;
    bits_t sPartialSum(sMul.size(), 0);

    bool sIsOverflow = false;
    while (!sBitsets.empty()) {
        auto sAction = Action::ADD;
        auto[sBitset, sIsCorrectionStep] = sBitsets.front();
        if (sIsOverflow) {
            sBitset = bits_t(sBitset.size(), 1);
            sIsOverflow = false;
        } else
            sBitsets.pop();
        std::string sVal1 = bitset2string(sPartialSum);
        std::string sVal2 = bitset2string(sBitset);
        sPartialSum = sPartialSum + sBitset;
        if (sBitset.none())
            sAction = Action::NONE;

        if (sPartialSum.size() > sMul.size()) {
            sIsOverflow = true;
            sPartialSum.pop_back();
        }

        sExps.emplace_back(Expression{
                .m_Val1 = sVal1,
                .m_Val2 = sVal2,
                .m_Action = sAction,
                .m_IsCorrectionStep = sIsCorrectionStep,
                .m_IsOverflow = sIsOverflow
        });
    }

    if (sIsOverflow) {
        sExps.emplace_back(Expression{
                .m_Val1 = bitset2string(sPartialSum),
                .m_Val2 = bitset2string(bits_t(sMul.size(), 1)),
                .m_Action = Action::ADD,
                .m_IsCorrectionStep = true,
                .m_IsOverflow = sIsOverflow
        });
        sPartialSum = sPartialSum + bits_t(sMul.size(), 1);
    }

    return Result{
            .m_Code = Code::REVERSE,
            .m_Method = Method::CORRECTION_STEP,
            .m_Val1 = bitset2string(aMul.bitset()).substr(aMul.size() - m_NumberSize),
            .m_Val2 = bitset2string(aFactor.bitset()).substr(aFactor.size() - m_NumberSize),
            .m_Result = bitset2string(sPartialSum),
            .m_IntermediateExps = std::move(sExps)
    };
}

Multiplier::Result Multiplier::mulWithAnalysisAdjacentBits(const AdditionalBitset & aMul,
                                                           const AdditionalBitset & aFactor) const {

    auto sMul = aMul;
    auto sFactor = aFactor;

    sMul <<= sMul.size() - (m_NumberSize);
    sFactor <<= sFactor.size() - (m_NumberSize);

    std::vector<bits_t> sBitsets;

    std::queue<Action> sActions;

    auto sReverseFactor = sFactor.reverse();
    for (size_t i = 0; i < m_NumberSize; i++) {
        auto sCurrent = sMul;
        auto sAction = bitOrderToAction(sReverseFactor[i], sReverseFactor[i + 1]);
        switch (sAction) {
            case Action::SUB:
                sCurrent = -sCurrent;
            case Action::ADD:
                sCurrent >>= i;
                sBitsets.emplace_back(sCurrent.bitset());
                break;
            default:
                sBitsets.emplace_back(sMul.size(), 0);
                break;
        }
        sActions.push(sAction);
    }

    std::vector<Expression> sExps;
    bits_t sPartialSum(sMul.size(), 0);

    for (auto & sBitset : sBitsets) {
        Expression sExp{
                .m_Val1 = bitset2string(sPartialSum),
                .m_Val2 = bitset2string(sBitset),
                .m_Action = sActions.front(),
                .m_IsCorrectionStep = false
        };
        sActions.pop();
        sPartialSum = sPartialSum + sBitset;
        if (sPartialSum.size() > sMul.size()) {
            sExp.m_IsOverflow = true;
            sPartialSum.pop_back();
        }
        sExps.emplace_back(std::move(sExp));
    }

    return Result{
            .m_Code = Code::ADDITIONAL,
            .m_Method = Method::ANALYSIS_ADJACENT_BITS,
            .m_Val1 = bitset2string(aMul.bitset()).substr(aMul.size() - m_NumberSize),
            .m_Val2 = bitset2string(aFactor.bitset()).substr(aFactor.size() - m_NumberSize),
            .m_Result = bitset2string(sPartialSum),
            .m_IntermediateExps = std::move(sExps)
    };
}

Multiplier::Result
Multiplier::mulWithInvertNumberBits(const ReverseBitset & aMul, const ReverseBitset & aFactor) const {
    auto sMul = aFactor.sign() ? -aMul : aMul;
    auto sFactor = aFactor.sign() ? -aFactor : aFactor;

    sMul <<= sMul.size() - (m_NumberSize);
    sFactor <<= sFactor.size() - (m_NumberSize);

    std::queue<bits_t> sBitsets;
    auto sReverseFactor = sFactor.reverse();
    for (size_t i = 1; i < m_NumberSize; i++) {

        if (!sReverseFactor[i]) {
            sBitsets.push(bits_t (sMul.size(), 0));
            continue;
        }
        auto sCurrent = sMul;
        sCurrent >>= i;
        sBitsets.push(sCurrent.bitset());
    }

    std::vector<Expression> sExps;
    bits_t sPartialSum(sMul.size(), 0);

    bool sIsOverflow = false;
    while (!sBitsets.empty()) {
        auto sAction = Action::ADD;
        auto sBitset= sBitsets.front();
        if (sIsOverflow) {
            sBitset = bits_t(sBitset.size(), 1);
            sIsOverflow = false;
        } else
            sBitsets.pop();
        std::string sVal1 = bitset2string(sPartialSum);
        std::string sVal2 = bitset2string(sBitset);
        sPartialSum = sPartialSum + sBitset;
        if (sBitset.none())
            sAction = Action::NONE;

        if (sPartialSum.size() > sMul.size()) {
            sIsOverflow = true;
            sPartialSum.pop_back();
        }

        sExps.emplace_back(Expression{
                .m_Val1 = sVal1,
                .m_Val2 = sVal2,
                .m_Action = sAction,
                .m_IsCorrectionStep = false,
                .m_IsOverflow = sIsOverflow
        });
    }

    if (sIsOverflow) {
        sExps.emplace_back(Expression{
                .m_Val1 = bitset2string(sPartialSum),
                .m_Val2 = bitset2string(bits_t(sMul.size(), 1)),
                .m_Action = Action::ADD,
                .m_IsCorrectionStep = false,
                .m_IsOverflow = sIsOverflow
        });
        sPartialSum = sPartialSum + bits_t(sMul.size(), 1);
    }

    return Result{
            .m_Code = Code::REVERSE,
            .m_Method = Method::INVERT_NUMBER_BITS,
            .m_Val1 = bitset2string(aMul.bitset()).substr(aMul.size() - m_NumberSize),
            .m_Val2 = bitset2string(aFactor.bitset()).substr(aFactor.size() - m_NumberSize),
            .m_Result = bitset2string(sPartialSum),
            .m_IntermediateExps = std::move(sExps)
    };
}

