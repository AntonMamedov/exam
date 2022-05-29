.PHONY: build_proto
build_proto:
	protoc --go_out=src/bin/tester/internal/app/v1/repository/grpc --go_opt=paths=source_relative --go-grpc_out=src/bin/tester/internal/app/v1/repository/grpc/ --go-grpc_opt=paths=source_relative protos/arithmetical.proto