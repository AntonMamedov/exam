package models

type Test struct {
	ID          uint64 `json:"id,omitempty" db:"id"`
	Number      uint64 `json:"number" db:"number"`
	Semester    uint64 `json:"semester" db:"semester"`
	Description string `json:"description" db:"description"`
}
