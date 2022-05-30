package repository

import (
	"context"
	arithmetical "tester/internal/app/v1/repository/grpc/protos"
	"tester/models"
)

type Profile interface {
	CreateProfile(ctx context.Context, profile models.Profile, pass string) (models.Profile, error)
	GetProfileWithPassByLogin(ctx context.Context, login string) (models.ProfileWithPassword, error)
	GetProfileById(ctx context.Context, ID uint64) (models.ProfileWithGroup, error)
}

type Group interface {
	CreateGroup(ctx context.Context, group models.Group) (models.Group, error)
	CreateProfileGroupRelation(ctx context.Context, profileID uint64, groupID uint64) error
	SelectGroupByProfileId(ctx context.Context, profileID uint64) (models.Group, error)
	SelectGroupByName(ctx context.Context, name string) (models.Group, error)
}

type Session interface {
	CreateSession(ctx context.Context, profileID uint64, userAgent string) (models.Session, error)
	SelectSessionByUUID(ctx context.Context, UUID string, userAgent string) (models.Session, error)
}

type Arithmetical interface {
	Mul(ctx context.Context, data models.MulRequest) (*arithmetical.MulResponse, error)
	MulOn2(ctx context.Context, mul int32, pow int32) (*arithmetical.CodeResponse, error)
	Code(ctx context.Context, num int32) (*arithmetical.CodeResponse, error)
}

type Repository interface {
	Profile
	Group
	Session
	Begin() (RepositoryTx, error)
}

type RepositoryTx interface {
	Repository
	Commit() error
	Rollback() error
}

type GrpcRepository interface {
	Arithmetical
}
