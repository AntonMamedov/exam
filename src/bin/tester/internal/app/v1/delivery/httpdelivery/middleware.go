package httpdelivery

import (
	"context"
	"errors"
	"github.com/rs/cors"
	"math/rand"
	"net/http"
	"strings"
	"tester/pkg/ctx"
	apperr "tester/pkg/error"
	"tester/pkg/logger"
)

const (
	authorizationHeader     = "Authorization"
	maxAgeDay           int = 86400
)

func (d Delivery) userIdentity(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		header := r.Header.Get(authorizationHeader)
		if header == "" {
			d.newErrorResponse(w, r, http.StatusUnauthorized, errors.New("empty auth header"))
			return
		}

		headerParts := strings.Split(header, " ")
		if len(headerParts) != 2 || headerParts[0] != "Bearer" {
			d.newErrorResponse(w, r, http.StatusUnauthorized, errors.New("invalid auth header"))
			return
		}

		if len(headerParts[1]) == 0 {
			d.newErrorResponse(w, r, http.StatusUnauthorized, errors.New("token is empty"))
			return
		}

		accessToken, err := d.services.Identity(r.Context(), headerParts[1])
		if err != nil && err == apperr.TokenExpired {
			d.newErrorResponse(w, r, http.StatusUnauthorized, err)
			return
		}

		if err != nil {
			d.newErrorResponse(w, r, http.StatusInternalServerError, err)
			return
		}

		next.ServeHTTP(w, r.WithContext(context.WithValue(r.Context(), ctx.TokenDataKey, accessToken)))
	})
}

func (d Delivery) loggingRequest(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		reqID := rand.Uint64()
		req := r.WithContext(context.WithValue(r.Context(), ctx.RequestIdKey, reqID))
		logger.LoggingRequest(req.Context(), r.URL, r.Method)
		next.ServeHTTP(w, req)
	})
}

func (d Delivery) CorsMiddleware(origin []string) *cors.Cors {
	return cors.New(cors.Options{
		AllowedMethods:   []string{"POST", "GET", "OPTIONS", "PUT", "DELETE", "PATCH"},
		AllowedHeaders:   []string{"Content-Type", "X-Requested-With", "Accept", "X-Csrf-Token", "Authorization"},
		ExposedHeaders:   []string{"X-Csrf-Token"},
		AllowCredentials: true,
		MaxAge:           maxAgeDay,
	})
}
