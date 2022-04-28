package postgres

import (
	"context"
	"errors"
	"github.com/jmoiron/sqlx"
	"github.com/lib/pq"
	"tester/models"
	"tester/pkg/logger"
)

const (
	insertProfile = `INSERT INTO profile (login, name, surname, password, role)
				     VALUES ($1, $2, $3, $4, $5) RETURNING id`
	selectProfileByLogin = `SELECT * FROM profile WHERE login = $1`
	selectProfileById    = `SELECT p.id, login, p.name, p.surname, p.role, COALESCE(g.name, '') AS "group"
							  FROM profile p
                              LEFT JOIN profile_group pg on p.id = pg.profile_id
                              LEFT JOIN "group" g on pg.group_id = g.id
                         WHERE p.id = $1`
)

type Profile struct {
	db sqlx.Ext
}

func NewProfilePostgres(db sqlx.Ext) (*Profile, error) {
	if db == nil {
		return nil, errors.New("expected db pointer, got nil")
	}

	return &Profile{db}, nil
}

func (p *Profile) CreateProfile(ctx context.Context, profile models.Profile, pass string) (models.Profile, error) {
	newProfile := profile
	err := p.db.QueryRowx(
		insertProfile,
		profile.Login,
		profile.Name,
		profile.Surname,
		pass,
		pq.Array(profile.Role)).Scan(&newProfile.ID)
	if err != nil {
		logger.LoggingError(ctx, logger.RepositoryLocation, err)
		return models.Profile{}, err
	}
	logger.LoggingDebug(ctx, logger.RepositoryLocation, map[string]interface{}{
		"Profile inserted in postgres": newProfile,
	})
	return newProfile, nil
}

func (p *Profile) GetProfileWithPassByLogin(ctx context.Context, login string) (models.ProfileWithPassword, error) {
	profile := models.ProfileWithPassword{}
	if err := sqlx.Get(p.db, &profile, selectProfileByLogin, login); err != nil {
		logger.LoggingError(ctx, logger.RepositoryLocation, err)
		return models.ProfileWithPassword{}, err
	}

	return profile, nil
}

func (p *Profile) GetProfileById(ctx context.Context, ID uint64) (models.ProfileWithGroup, error) {
	profile := models.ProfileWithGroup{}
	if err := sqlx.Get(p.db, &profile, selectProfileById, ID); err != nil {
		logger.LoggingError(ctx, logger.RepositoryLocation, err)
		return models.ProfileWithGroup{}, err
	}

	return profile, nil
}
