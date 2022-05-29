package postgres

import (
	"context"
	"errors"
	"github.com/jmoiron/sqlx"
	"tester/models"
	"tester/pkg/logger"
)

const (
	insertSession = `INSERT INTO session (profile_id, user_agent) 
                          VALUES ($1, $2) RETURNING uuid, profile_id, user_agent, expires`
	selectSession = `SELECT * FROM session WHERE uuid = $1 AND user_agent = $2`
)

type Session struct {
	db sqlx.Ext
}

func NewSessionPostgres(db sqlx.Ext) (*Session, error) {
	if db == nil {
		return nil, errors.New("expected db, got nil")
	}

	return &Session{db: db}, nil
}

func (s Session) CreateSession(ctx context.Context, profileID uint64, userAgent string) (models.Session, error) {
	session := models.Session{}
	err := s.db.QueryRowx(insertSession, profileID, userAgent).StructScan(&session)
	if err != nil {
		logger.LoggingError(ctx, logger.RepositoryLocation, err)
		return models.Session{}, err
	}

	logger.LoggingDebug(ctx, logger.RepositoryLocation, map[string]interface{}{
		"Refresh token meta inserted in postgres": session,
	})

	return session, err
}

func (s Session) SelectSessionByUUID(ctx context.Context, UUID string, userAgent string) (models.Session, error) {
	session := models.Session{}
	err := s.db.QueryRowx(selectSession, UUID, userAgent).StructScan(&session)
	if err != nil {
		logger.LoggingError(ctx, logger.RepositoryLocation, err)
		return models.Session{}, err
	}

	return session, nil
}
