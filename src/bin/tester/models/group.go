package models

type Group struct {
	ID         uint64 `db:"id" json:"id,omitempty"`
	Department string `db:"department" json:"department,omitempty"`
	Cathedra   int32  `db:"cathedra" json:"cathedra,omitempty"`
	Semester   string `db:"semester" json:"semester,omitempty"`
	GroupID    string `db:"group_id" json:"group_id,omitempty"`
	Name       string `db:"name" json:"name,omitempty"`
}
