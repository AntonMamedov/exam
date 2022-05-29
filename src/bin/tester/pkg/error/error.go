package error

import (
	"database/sql"
	"errors"
	"github.com/lib/pq"
)

var (
	DuplicateError           = errors.New("duplicate record")
	RecordNotFound           = errors.New("record not found")
	InternalError            = errors.New("internal error")
	BadRoleError             = errors.New("bad role")
	BadPasswordError         = errors.New("bad password")
	TokenExpired             = errors.New("token is expired")
	InvalidRefreshTokenError = errors.New("invalid refresh token")
)

const (
	PGDuplicateErrorCode = "23505"
)

func NewServiceError(err error) error {
	if err == nil {
		return nil
	}

	if err == TokenExpired {
		return err
	}

	// Блок проверки ошибок, созданных sqlx
	if err == sql.ErrNoRows {
		return RecordNotFound
	}

	// Блок проверки ошибок базы
	pqErr := &pq.Error{}
	if !errors.As(err, &pqErr) {
		return InternalError
	}

	if pqErr.Code == PGDuplicateErrorCode {
		return DuplicateError
	}

	return InternalError
}
