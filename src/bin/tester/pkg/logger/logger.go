package logger

import (
	"context"
	log "github.com/sirupsen/logrus"
	"net/url"
	"os"
	ctx2 "tester/pkg/ctx"
)

const (
	DeliveryLocation   = "DELIVERY"
	ServiceLocation    = "SERVICE"
	RepositoryLocation = "REPOSITORY"
)

func InitLogger(stream string) error {
	if stream == "stdout" {
		log.SetOutput(os.Stdout)
	} else {
		logFileStream, err := os.Open(stream)
		if err != nil {
			return err
		}
		log.SetOutput(logFileStream)
	}

	return nil
}

func LoggingRequest(ctx context.Context, url *url.URL, method string) {
	requestID := ctx.Value(ctx2.RequestIdKey).(uint64)
	log.WithFields(log.Fields{
		"request_id": requestID,
		"url":        url,
		"method":     method,
	}).Info()
}

func LoggingResponse(ctx context.Context, location string, code int) {
	requestID := ctx.Value(ctx2.RequestIdKey).(uint64)
	log.WithFields(log.Fields{
		"request_id": requestID,
		"location":   location,
		"reply_code": code,
	}).Info()
}

func LoggingError(ctx context.Context, location string, err error) {
	requestID := ctx.Value(ctx2.RequestIdKey).(uint64)
	log.WithFields(log.Fields{
		"request_id": requestID,
		"location":   location,
		"error":      err,
	}).Error()
}

func LoggingDebug(ctx context.Context, location string, msg interface{}) {
	requestID := ctx.Value(ctx2.RequestIdKey).(uint64)
	log.WithFields(log.Fields{
		"request_id": requestID,
		"location":   location,
		"message":    msg,
	}).Debug()
}
