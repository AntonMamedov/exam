#include "engine.hpp"
#include "bitset.hpp"

#include <queue>
#include <stack>
#include <optional>

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

    Action bitOrderToAction(bit_t aBi1, bit_t aBit2) {
        if (!aBi1 && aBit2)
            return Action::ADD;
        else if (aBi1 && !aBit2)
            return Action::SUB;
        return Action::NONE;
    }


    int bitOrderToMulOnTwoFactorBitsAction(bit_t aLeft, bit_t aRight) {
        int sAction;
        if (!aLeft && !aRight) {
            sAction = 0;
        } else if (!aLeft && aRight) {
            sAction = 1;
        } else if (aLeft && !aRight) {
            sAction = 2;
        } else {
            sAction = 3;
        }
        return sAction;
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
            sBitsets.push(bits_t(sMul.size(), 0));
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
        auto sBitset = sBitsets.front();
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

Multiplier::Result Multiplier::mulOnTwoFactorBits(const DirectBitset & aMul, const DirectBitset & aFactor) const {
    auto sFactorBitset = aFactor.reverse().bitset();
    auto sMul = aMul;
    sMul <<= sMul.size() - (m_NumberSize);

    bool sFactorIsNegative = false;
    bool sMulIsNegative = false;

    if (aMul[aMul.size() - 1]) {
        sMulIsNegative = true;
        sMul = -sMul;
    }

    if (aFactor[aFactor.size() - 1]) {
        sFactorIsNegative = true;
        sFactorBitset[sFactorBitset.size() - 1] = false;
    }

    sFactorBitset >>= m_NumberSize;
    sFactorBitset.resize(sFactorBitset.size() - m_NumberSize);
    std::vector<int> sActionsList{0};
    auto sFactorSize = sFactorBitset.size();
    for (size_t i = 0; i < sFactorSize; i += 2) {
        auto sAction = bitOrderToMulOnTwoFactorBitsAction(sFactorBitset[i], i < sFactorSize && sFactorBitset[i + 1]);
        sActionsList.emplace_back(sAction);
    }

    int sTransfer = 0;
    for (int i = sActionsList.size() - 1; i >= 0; i--) {
        sActionsList[i] += sTransfer;
        sTransfer = 0;
        if (sActionsList[i] > 2) {
            sTransfer = (sActionsList[i] + 1) / 4;
            sActionsList[i] = -1 + (sActionsList[i] + 1) % 4;
        }
    }

    std::stack<int> sActions;
    for (int sAct : sActionsList) {
        sActions.push(sAct);
    }


    std::vector<std::optional<bits_t>> sBitsets;
    while (!sActions.empty()) {
        int sAct = sActions.top();
        sActions.pop();
        switch (sAct) {
            case 0:
                sBitsets.emplace_back(std::nullopt);
                break;
            case 1:
                sBitsets.emplace_back(sMul.bitset());
                break;
            case 2: {
                auto sBitset = sMul.bitset();
                sBitset.push_back(OFF);
                sBitset <<= 1;
                sBitsets.emplace_back(sBitset);
                break;
            }
            case -1: {
                AdditionalBitset sAdditionalMul(static_cast<int>(sMul), sMul.size() - 1);
                sBitsets.emplace_back((-sAdditionalMul).bitset());
                break;
            }
            default:
                assert(false && "sAct != 0, 1, 2, -1");
        }
    }

    std::vector<Expression> sExps;
    bits_t sPartialSum(aMul.size(), 0);
    for (auto & sBitset : sBitsets) {
        bits_t sMask(sPartialSum.size(), sPartialSum[sPartialSum.size() - 1] ? BIT_MASK : 0);
        sPartialSum >>= 2;
        sMask <<= sMask.size() - 2;
        sPartialSum |= sMask;
        if (sBitset == std::nullopt) {
            continue;
        }
        bit_t sBitsetSign = (*sBitset)[sBitset->size() - 1];
        if (sPartialSum.size() > aMul.size())
            sPartialSum.pop_back();
        if (sBitset->size() > aMul.size())
            sBitset->pop_back();
        std::string sVal1 = bitset2string(sPartialSum);
        std::string sVal2 = bitset2string(*sBitset);
        sPartialSum = sPartialSum + *sBitset;
        sExps.emplace_back(Expression{
                .m_Val1 = sVal1,
                .m_Val2 = sVal2,
                .m_Action = {},
                .m_IsCorrectionStep = false,
                .m_IsOverflow = false
        });
        if (sMask.none() && !sBitsetSign && sPartialSum[sPartialSum.size() - 1]) {
            sPartialSum.push_back(OFF);
        }
    }

    if (sPartialSum.size() > aMul.size()) {
        sPartialSum.pop_back();
    }

    bool sResSign = false;
    if (sFactorIsNegative == sMulIsNegative == 1) {
        sResSign = false;
    } else if ((sFactorIsNegative | sMulIsNegative) == 1) {
        sResSign = true;
    }
    sPartialSum[sPartialSum.size() - 1] = sResSign;

    return Result{
            .m_Code = Code::DIRECT,
            .m_Method = Method::ON_TWO_FACTOR_BITS,
            .m_Val1 = bitset2string(aMul.bitset()).substr(aMul.size() - m_NumberSize),
            .m_Val2 = bitset2string(aFactor.bitset()).substr(aFactor.size() - m_NumberSize),
            .m_Result = bitset2string(sPartialSum),
            .m_IntermediateExps = std::move(sExps)
    };
}

Multiplier::Result Multiplier::mulFromLowBits(const DirectBitset & aMul, const DirectBitset & aFactor) const {
    auto sMul = aMul;
    auto sFactor = aFactor;

    bool sMulIsNegative = sMul.sign();
    bool sFactorIsNegative = sFactor.sign();

    if (sMulIsNegative)
        sMul = -sMul;

    if (sFactorIsNegative)
        sFactor = -sFactor;

    sMul <<= sMul.size() - (m_NumberSize);
    sFactor <<= sFactor.size() - (m_NumberSize);
    std::queue<Action> sActions;
    for (size_t i = sFactor.size() - m_NumberSize; i < sFactor.size(); i++) {
        if (!sFactor[i]) {
            sActions.push(Action::NONE);
        } else {
            sActions.push(Action::ADD);
        }
    }

    bits_t sPartialSum(sMul.size(), 0);
    bits_t sRes(sMul.size(), 0);
    std::vector<Expression> sExps;
    while (!sActions.empty()) {
        auto sAction = sActions.front();
        sActions.pop();
        sPartialSum = sRes;
        switch (sAction) {
            case Action::ADD:
                sRes= sPartialSum + sMul.bitset();
            case Action::NONE:
                sRes >>= 1;
                break;
            default:
                assert(false && "incorrect Action");
        }
        sExps.emplace_back(Expression{
                .m_Val1 = bitset2string(sPartialSum),
                .m_Val2 = bitset2string(sMul.bitset()),
                .m_Action = sAction,
                .m_IsCorrectionStep = false,
                .m_IsOverflow = false
        });
    }

    bool sResSign = false;
    if (sFactorIsNegative == sMulIsNegative == 1) {
        sResSign = false;
    } else if ((sFactorIsNegative | sMulIsNegative) == 1) {
        sResSign = true;
    }

    sPartialSum[sPartialSum.size() - 1] = sResSign;
    return Result{
            .m_Code = Code::DIRECT,
            .m_Method = Method::FROM_LOW_BITS,
            .m_Val1 = bitset2string(aMul.bitset()).substr(aMul.size() - m_NumberSize),
            .m_Val2 = bitset2string(aFactor.bitset()).substr(aFactor.size() - m_NumberSize),
            .m_Result = bitset2string(sPartialSum),
            .m_IntermediateExps = std::move(sExps)
    };
}

Multiplier::Result Multiplier::mulFromHighBits(const DirectBitset & aMul, const DirectBitset & aFactor) const {
    auto sMul = aMul;
    auto sFactor = aFactor;

    bool sMulIsNegative = sMul.sign();
    bool sFactorIsNegative = sFactor.sign();

    if (sMulIsNegative)
        sMul = -sMul;

    if (sFactorIsNegative)
        sFactor = -sFactor;

    sMul <<= sMul.size() - (m_NumberSize);
    sFactor <<= sFactor.size() - (m_NumberSize);
    sMul >>= sMul.size() - (m_NumberSize);
    sFactor >>= sFactor.size() - (m_NumberSize);
    std::cout << sMul.bitset() << std::endl;
    std::cout << sFactor.bitset() << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::queue<Action> sActions;
    for (int i = m_NumberSize - 2; i >= 0; i--) {
        if (!sFactor[i]) {
            sActions.push(Action::NONE);
        } else {
            sActions.push(Action::ADD);
        }
    }

    bits_t sPartialSum(sMul.size(), 0);
    bits_t sRes(sMul.size(), 0);
    std::vector<Expression> sExps;
    while (!sActions.empty()) {
        auto sAction = sActions.front();
        sActions.pop();
        sPartialSum = sRes;
        switch (sAction) {
            case Action::ADD:
                sRes= sPartialSum + sMul.bitset();
            case Action::NONE:
                sRes <<= 1;
                break;
            default:
                assert(false && "incorrect Action");
        }
        std::cout << sPartialSum << std::endl;
        std::cout << sMul.bitset() << std::endl;
        std::cout << "-----------------------------" << std::endl;
        sExps.emplace_back(Expression{
                .m_Val1 = bitset2string(sPartialSum),
                .m_Val2 = bitset2string(sMul.bitset()),
                .m_Action = sAction,
                .m_IsCorrectionStep = false,
                .m_IsOverflow = false
        });
    }
    sRes >>= 1;
    bool sResSign = false;
    if (sFactorIsNegative == sMulIsNegative == 1) {
        sResSign = false;
    } else if ((sFactorIsNegative | sMulIsNegative) == 1) {
        sResSign = true;
    }

    sRes[sPartialSum.size() - 1] = sResSign;
    return Result{
            .m_Code = Code::DIRECT,
            .m_Method = Method::FROM_HIGH_BITS,
            .m_Val1 = bitset2string(aMul.bitset()).substr(aMul.size() - m_NumberSize),
            .m_Val2 = bitset2string(aFactor.bitset()).substr(aFactor.size() - m_NumberSize),
            .m_Result = bitset2string(sRes),
            .m_IntermediateExps = std::move(sExps)
    };
}
