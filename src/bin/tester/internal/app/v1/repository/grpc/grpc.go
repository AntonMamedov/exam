package grpc

import (
	arithmetical "tester/internal/app/v1/repository/grpc/protos"
)

type Repository struct {
	Arithmetical
	cl arithmetical.ArithmeticalClient
}

func NewGrpcRep(cl arithmetical.ArithmeticalClient) *Repository {
	return &Repository{
		Arithmetical: Arithmetical{cl: cl},
		cl:           cl,
	}
}
