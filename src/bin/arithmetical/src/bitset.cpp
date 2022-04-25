#include "bitset.hpp"

using namespace arithmetical::details;

namespace {
    constexpr const size_t MINIMAL_GRID_SIZE = 2;
    constexpr const auto MINIMAL_NUMBER_SIZE_ERROR = "number must be greater than 2\n";
    constexpr const auto DIFFERENT_CODES_ERRORS = "added values cannot be represented in different codes\n";
    constexpr const auto BIT_MASK = 0xFFFFFFFFFFFFFFFFUL;
}

DirectBitset::DirectBitset(int aData, size_t aGridSize) {
    m_GridSize = aGridSize;

    if (aGridSize < MINIMAL_GRID_SIZE)
        throw std::logic_error(MINIMAL_NUMBER_SIZE_ERROR);
    m_SignBit = OFF;
    if (aData < 0) {
        m_SignBit = ON;
        aData = -aData;
    }

    m_Data = bits_t(aGridSize, aData);
}

void DirectBitset::leftShift(size_t aPos) {
    m_Data <<= aPos;
}

void DirectBitset::rightShift(size_t aPos) {
    m_Data >>= aPos;
}


iBitset::ptr DirectBitset::invert() const {
    auto sInvertBitset = std::make_unique<DirectBitset>(*this);
    sInvertBitset->m_Data = ~sInvertBitset->m_Data;
    sInvertBitset->m_SignBit = ~sInvertBitset->m_SignBit;
    return std::move(sInvertBitset);
}

int DirectBitset::toInt() const {
    bits_t sDataBits = m_Data;
    int sData = static_cast<int>(sDataBits.to_ulong());

    return m_SignBit == OFF ? sData : -sData;
}


iBitset::ptr DirectBitset::invertSign() const {
    auto sBitset = std::make_unique<DirectBitset>(*this);
    sBitset->m_SignBit = ~sBitset->m_SignBit;
    return std::move(sBitset);
}

ReverseBitset::ReverseBitset(int aData, size_t aGridSize) {
    m_GridSize = aGridSize;
    if (aGridSize < MINIMAL_GRID_SIZE)
        throw std::logic_error(MINIMAL_NUMBER_SIZE_ERROR);
    m_SignBit = OFF;
    if (aData < 0) {
        aData -= 1;
        m_SignBit = ON;
    }
    m_Data = bits_t(m_GridSize, aData);
}


void ReverseBitset::leftShift(size_t aPos) {
    m_Data <<= aPos;
}

void ReverseBitset::rightShift(size_t aPos) {
    m_Data >>= aPos;
    if (!m_SignBit) {
        return;
    }
    bits_t aMask(m_GridSize, BIT_MASK);
    aMask <<= (m_GridSize) - std::min(aPos, m_GridSize);
    m_Data |= aMask;
}

iBitset::ptr ReverseBitset::invert() const {
    auto sBitset = std::make_unique<ReverseBitset>(*this);
    sBitset->m_Data = ~sBitset->m_Data;
    sBitset->m_SignBit = ~sBitset->m_SignBit;
    return std::move(sBitset);
}

int ReverseBitset::toInt() const {
    if (!m_SignBit) {
        return static_cast<int>(m_Data.to_ulong());
    }
    auto sData = ~m_Data;
    return -static_cast<int>(sData.to_ulong());
}


iBitset::ptr ReverseBitset::invertSign() const {
    return this->invert();
}

AdditionalBitset::AdditionalBitset(int aData, size_t aGridSize) {
    m_GridSize = aGridSize;
    if (aGridSize < MINIMAL_GRID_SIZE)
        throw std::logic_error(MINIMAL_NUMBER_SIZE_ERROR);
    m_SignBit = OFF;
    if (aData < 0) {
        m_SignBit = ON;
    }
    m_Data = bits_t(aGridSize, aData);
}

void AdditionalBitset::leftShift(size_t aPos) {
    m_Data <<= aPos;
    if (m_Data.none()) {
        m_SignBit = OFF;
    }
}

void AdditionalBitset::rightShift(size_t aPos) {
    m_Data >>= aPos;
    if (!m_SignBit) {
        return;
    }
    bits_t aMask(m_GridSize, BIT_MASK);
    aMask <<= (m_GridSize) - std::min(aPos, m_GridSize);
    m_Data |= aMask;
}


iBitset::ptr AdditionalBitset::invert() const {
    auto sBitset = std::make_unique<AdditionalBitset>(*this);
    sBitset->m_Data = ~sBitset->m_Data;
    sBitset->m_SignBit = ~sBitset->m_SignBit;
    return std::move(sBitset);
}

#include "iostream"
int AdditionalBitset::toInt() const {
    if (!m_SignBit)
        return static_cast<int>(m_Data.to_ulong());

    return -static_cast<int>((~m_Data).to_ulong() + 1);
}

iBitset::ptr AdditionalBitset::invertSign() const {
    auto sBitset = std::make_unique<AdditionalBitset>(*this);
    uint64_t sData = sBitset->m_Data.to_ulong();
    sData = ~sData;
    sData += 1;
    sBitset->m_Data = bits_t(m_GridSize, sData);
    return std::move(sBitset);
}
