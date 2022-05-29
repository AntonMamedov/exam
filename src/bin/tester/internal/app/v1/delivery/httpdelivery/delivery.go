package httpdelivery

import (
	"errors"
	"github.com/gorilla/mux"
	"net/http"
	"tester/internal/app/v1/service"
)

var (
	Origin = []string{"httpdelivery://localhost", "httpdelivery://localhost:80", "httpdelivery://localhost:8000", "httpdelivery://192.168.1.114:8000", "httpdelivery://192.168.1.114:80"}
)

type Delivery struct {
	services *service.Service
}

func NewHTTPDelivery(services *service.Service) (*Delivery, error) {
	if services == nil {
		return nil, errors.New("expected services, got nil")
	}

	return &Delivery{services: services}, nil
}

func (d *Delivery) InitRoutes() *mux.Router {
	router := mux.NewRouter()
	router.Use(d.loggingRequest)
	apiRoute := router.PathPrefix("/api/v1").Subrouter()
	apiRoute.HandleFunc("/signup", d.signUp).Methods(http.MethodPost)
	apiRoute.HandleFunc("/signin", d.signIn).Methods(http.MethodPost)
	apiRoute.HandleFunc("/refresh", d.refresh).Methods(http.MethodPost)
	apiRoute.HandleFunc("/mul", d.mul).Methods(http.MethodPost)
	apiRoute.HandleFunc("/getcodes", d.code).Methods(http.MethodPost)
	profileRoute := apiRoute.PathPrefix("/profile").Subrouter()
	profileRoute.Use(d.userIdentity)
	profileRoute.HandleFunc("/auth", d.auth).Methods(http.MethodGet)
	return router
}
