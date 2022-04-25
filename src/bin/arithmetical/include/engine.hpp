#pragma once

#include <memory>
#include <vector>

namespace arithmetical::details {
    class iBitset;

    enum class Code;
    using bit_t = bool;
}

namespace arithmetical::engine {

    enum class Action {
        ADD = 1,
        SUB,
        MUL,
        DIV
    };


    class Multiplier {
    public:
        struct Expression {
            std::string m_Val1;
            std::string m_Val2;
            Action m_Action;
            bool m_IsCorrectionStep;
        };

        struct Result {
            details::Code m_Code;
            std::string m_Val1;
            std::string m_Val2;
            std::string m_Result;
            std::vector<Expression> m_IntermediateExps;
        };
        explicit Multiplier(size_t aNumberSize) : m_NumberSize(aNumberSize) {}

        Multiplier::Result mulWithCurrentStep(const std::unique_ptr<details::iBitset> & aMul,
                                              const std::unique_ptr<details::iBitset> & aFactor);
    private:
        size_t m_NumberSize;
    };
}
