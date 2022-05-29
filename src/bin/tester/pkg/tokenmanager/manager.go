package tokenmanager

import (
	"errors"
	"fmt"
	"github.com/dgrijalva/jwt-go"
	"tester/models"
	"time"
)

type TokenManager interface {
	NewAccessJWT(payload models.AccessToken, ttl time.Duration) (string, models.AccessToken, error)
	ParseAccessToken(accessToken string) (models.AccessToken, error)
}

type Manager struct {
	signingAccessKey string
}

func NewManager(signingAccessKey string) (*Manager, error) {
	if signingAccessKey == "" {
		return nil, errors.New("empty signing key")
	}

	return &Manager{
		signingAccessKey: signingAccessKey,
	}, nil
}

func (m Manager) NewAccessJWT(payload models.AccessToken, ttl time.Duration) (string, models.AccessToken, error) {
	claims := &Claims{AccessToken: payload}
	claims.Exp = time.Now().Add(ttl).Unix()

	t := jwt.NewWithClaims(jwt.SigningMethodHS256, claims)
	signedToken, err := t.SignedString([]byte(m.signingAccessKey))

	if err != nil {
		return "", models.AccessToken{}, err
	}

	return signedToken, claims.AccessToken, nil
}

func (m *Manager) ParseAccessToken(accessToken string) (models.AccessToken, error) {
	claims := Claims{}
	_, err := jwt.ParseWithClaims(accessToken, &claims, func(token *jwt.Token) (i interface{}, err error) {
		if _, ok := token.Method.(*jwt.SigningMethodHMAC); !ok {
			return nil, fmt.Errorf("unexpexted signing method: %s", token.Header["alg"])
		}
		return []byte(m.signingAccessKey), nil
	})
	if err != nil {
		return models.AccessToken{}, err
	}

	return claims.AccessToken, nil
}
