package service

import (
	"context"
	"errors"
	"tester/internal/app/v1/repository"
	arithmetical "tester/internal/app/v1/repository/grpc/protos"
	"tester/models"
	"tester/pkg/logger"
)

type Auth interface {
	SignUpTeacher(ctx context.Context, reg models.Registration, userAgent string) (models.AuthorizationResponse, error)
	SignUpStudent(ctx context.Context, reg models.Registration, userAgent string) (models.AuthorizationResponse, error)
	SignIn(ctx context.Context, auth models.AuthorizationWithUserAgent) (models.AuthorizationResponse, error)
	Identity(ctx context.Context, accessToken string) (models.AccessToken, error)
	Refresh(ctx context.Context, UUID string, userAgent string) (string, string, error)
}

type Arithmetical interface {
	Mul(ctx context.Context, mul models.MulRequest) (models.MulResponse, error)
}

type Service struct {
	Auth
	Arithmetical
}

func NewService(r repository.Repository, cl arithmetical.ArithmeticalClient) (*Service, error) {
	if r == nil {
		return nil, errors.New("expected repository, got nil")
	}
	a, err := NewSessionService(r)
	if err != nil {
		return nil, err
	}

	ar, err := NewArithmeticalService(r, cl)
	return &Service{Auth: a, Arithmetical: ar}, nil
}

func processingEndTx(ctx context.Context, tx repository.RepositoryTx, err *error) {
	var txErr error
	if *err != nil {
		txErr = tx.Rollback()
		if txErr != nil {
			logger.LoggingError(ctx, logger.ServiceLocation, txErr)
		}
		return
	}

	txErr = tx.Commit()
	if txErr != nil {
		logger.LoggingError(ctx, logger.ServiceLocation, txErr)
	}
}
