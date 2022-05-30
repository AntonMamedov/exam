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

func (a Arithmetical) Code(ctx context.Context, num int32) (*arithmetical.CodeResponse, error) {
	req := &arithmetical.CodeRequest{
		Num: num,
	}

	resp, err := a.cl.Code(ctx, req)
	if err != nil {
		return nil, err
	}
	return resp, nil
}

func (a Arithmetical) MulOn2(ctx context.Context, mul int32, pow int32) (*arithmetical.CodeResponse, error) {
	req := &arithmetical.MulOn2Request{
		Mul: mul,
		Pow: pow,
	}

	resp, err := a.cl.MulOn2(ctx, req)
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
			methodRes = arithmetical.MulRequest_MUL_ON_2_FACTOR_BITS
			break
		default:
			panic(fmt.Sprintf("method %d dose not exists", method))
		}
	}
	return methodRes
}
