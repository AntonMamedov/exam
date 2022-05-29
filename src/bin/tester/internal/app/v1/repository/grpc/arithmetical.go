package grpc

import (
	"context"
	"fmt"
	"google.golang.org/grpc"
	arithmetical "tester/internal/app/v1/repository/grpc/protos"
	"tester/models"
)

type Arithmetical struct {
	cl arithmetical.ArithmeticalClient
}

func (a Arithmetical) Mul(ctx context.Context, data models.MulRequest) (*arithmetical.MulResponse, error) {
	req := &arithmetical.MulRequest{
		Mul:        data.Mul,
		Factor:     data.Factor,
		NumberSize: data.NumberSize,
		GridSize:   data.GridSize,
		Method:     getMethod(data.Code, data.Method),
	}
	resp, err := a.cl.Mul(ctx, req, grpc.EmptyCallOption{})
	if err != nil {
		return nil, err
	}
	return resp, nil
}

func getMethod(code models.CodeEnum, method models.MulMethodEnum) arithmetical.MulRequest_Method {

	var methodRes arithmetical.MulRequest_Method

	if code == models.Direct {
		switch method {
		case 0:
			break
		case 1:
			break
		case 2:
			methodRes = arithmetical.MulRequest_ANALYSIS_ADJACENT_BITS
			break
		default:
			panic(fmt.Sprintf("method %d dose not exists", method))
		}
	}
	return methodRes
}
