package postgres

import (
	"context"
	"errors"
	"github.com/jmoiron/sqlx"
	"tester/models"
	"tester/pkg/logger"
)

const (
	insertProfileGroup     = `INSERT INTO profile_group (profile_id, group_id) VALUES ($1, $2)`
	selectGroupByProfileId = `SELECT g.id, g.name FROM 
              				  profile_group AS pg
              				  LEFT JOIN group AS g ON pg.group_id = g.id WHERE pg.profile_id = $1`
)

type Group struct {
	db sqlx.Ext
}

func NewGroupPostgres(db sqlx.Ext) (*Group, error) {
	if db == nil {
		return nil, errors.New("expected db pointer, got nil")
	}

	return &Group{db}, nil
}

func (g *Group) CreateProfileGroupRelation(ctx context.Context, profileID uint64, groupID uint64) error {
	_, err := g.db.Queryx(insertProfileGroup, profileID, groupID)
	if err != nil {
		logger.LoggingError(ctx, logger.RepositoryLocation, err)
	}

	return err
}

func (g *Group) SelectGroupByProfileId(ctx context.Context, profileID uint64) (models.Group, error) {
	group := models.Group{}
	err := sqlx.Get(g.db, &group, selectGroupByProfileId, profileID)
	if err != nil {
		logger.LoggingError(ctx, logger.RepositoryLocation, err)
		return models.Group{}, err
	}

	return group, err
}
