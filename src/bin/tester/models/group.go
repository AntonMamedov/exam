package models

type Group struct {
	ID   uint64 `db:"id"`
	Name string `db:"name"`
}
