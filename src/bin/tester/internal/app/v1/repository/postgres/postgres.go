package postgres

import (
	"errors"
	"fmt"
	"github.com/jmoiron/sqlx"
	"tester/pkg/config"
)

type Repository struct {
	Profile *Profile
	Session *Session
	Group   *Group
	Test    *Test
	db      *sqlx.DB
}

func NewPostgresRepository(db *sqlx.DB) (*Repository, error) {
	if db == nil {
		return nil, errors.New("db is nil")
	}
	profile, _ := NewProfilePostgres(db)
	group, _ := NewGroupPostgres(db)
	session, _ := NewSessionPostgres(db)
	test, _ := NewTest(db)
	return &Repository{
		db:      db,
		Profile: profile,
		Group:   group,
		Session: session,
		Test:    test,
	}, nil
}

func (r Repository) Begin() (*RepositoryTx, error) {
	tx, err := r.db.Beginx()
	if err != nil {
		return nil, err
	}

	return &RepositoryTx{
		tx:         tx,
		Repository: r,
	}, nil
}

type RepositoryTx struct {
	Repository
	tx *sqlx.Tx
}

func (r RepositoryTx) Commit() error {
	return r.tx.Commit()
}

func (r RepositoryTx) Rollback() error {
	return r.tx.Rollback()
}

func NewPostgresDB(cfg config.PostgresConfig) (*sqlx.DB, error) {
	db, err := sqlx.Connect("postgres", fmt.Sprintf("host=%s port=%s user=%s dbname=%s password=%s sslmode=%s",
		cfg.Host, cfg.Port, cfg.Username, cfg.DBName, cfg.Password, cfg.SSLMode))
	if err != nil {
		return nil, err
	}

	err = db.Ping()
	if err != nil {
		return nil, err
	}

	return db, nil
}
