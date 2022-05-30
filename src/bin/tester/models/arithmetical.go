package models

import (
	"tester/pkg/enum"
)

type MulMethodEnum enum.Enum

const (
	CorrectionStep MulMethodEnum = iota
	AnalysisAdjacentBits
	InvertNumberBits
	FromLowBitsWithLeftShift
	FromHighBitsWithRightShift
)

type CodeEnum enum.Enum

const (
	Direct CodeEnum = iota
	Reverse
	Additional
)

type AddRequest struct {
	Val1     int32    `json:"val1"`
	Val2     int32    `json:"val2"`
	Code     CodeEnum `json:"code"`
	GridSize int32    `json:"grid_ize"`
}

type MulRequest struct {
	Mul        int32         `json:"mul"`
	Factor     int32         `json:"factor"`
	NumberSize int32         `json:"number_size"`
	GridSize   int32         `json:"grid_size"`
	Method     MulMethodEnum `json:"method"`
	Code       CodeEnum      `json:"code"`
}

type Add struct {
	Term1      int32    `json:"term_1"`
	Term2      int32    `json:"term_2"`
	NumberSize int32    `json:"number_size"`
	GridSize   int32    `json:"grid_size"`
	Code       CodeEnum `json:"code"`
}

type Codes struct {
	Direct     string `json:"direct"`
	Reverse    string `json:"reverse"`
	Additional string `json:"additional"`
}

type MulExp struct {
	Val1             string `json:"val1"`
	Val2             string `json:"val2"`
	IsOverflow       bool   `json:"is_overflow"`
	IsCorrectionStep bool   `json:"is_correction_step"`
}

type MulResponse struct {
	Mul    string   `json:"mul"`
	Factor string   `json:"factor"`
	Result string   `json:"result"`
	Exps   []MulExp `json:"exps"`
}

type MulOn2Request struct {
	Mul int32 `json:"mul"`
	Pow int32 `json:"pow"`
}
