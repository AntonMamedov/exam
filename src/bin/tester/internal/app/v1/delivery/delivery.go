package delivery

import (
	"errors"
	"github.com/gorilla/mux"
	"github.com/rs/cors"
	"tester/internal/app/v1/delivery/httpdelivery"
	"tester/internal/app/v1/service"
)

type HTTP interface {
	InitRoutes() *mux.Router
	CorsMiddleware(origin []string) *cors.Cors
}

type Delivery struct {
	HTTP
}

func NewDelivery(service *service.Service) (*Delivery, error) {
	if service == nil {
		return nil, errors.New("expected service, got null")
	}
	d, _ := httpdelivery.NewHTTPDelivery(service)
	return &Delivery{HTTP: d}, nil
}
