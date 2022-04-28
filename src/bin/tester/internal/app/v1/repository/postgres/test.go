package postgres

import (
	"errors"
	"github.com/jmoiron/sqlx"
)

type Test struct {
	db sqlx.Ext
}

func NewTest(db sqlx.Ext) (*Test, error) {
	if db == nil {
		return nil, errors.New("expected db pointer, got nil")
	}

	return &Test{db}, nil
}
