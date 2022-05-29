package tokenmanager

import (
	"tester/models"
	apperr "tester/pkg/error"
	"time"
)

type Claims struct {
	models.AccessToken
	UUID string `json:"uuid,omitempty"`
}

func (c Claims) Valid() error {
	now := time.Now().Unix()
	if ok := c.verifyExpiresAt(now, false); !ok {
		return apperr.TokenExpired
	}

	return nil
}

func (c Claims) verifyExpiresAt(now int64, req bool) bool {
	if c.Exp == 0 {
		return !req
	}

	return now <= c.Exp
}
