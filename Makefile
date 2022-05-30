.PHONY: build_proto
build_proto:
	protoc --go_out=src/bin/tester/internal/app/v1/repository/grpc --go_opt=paths=source_relative --go-grpc_out=src/bin/tester/internal/app/v1/repository/grpc/ --go-grpc_opt=paths=source_relative protos/arithmetical.proto

.PHONY: build
build:
	make build_proto
	mkdir bin
	mkdir build && cd build && cmake ../src/bin/arithmetical && make && mv arithmetical ../bin
	cd src/bin/tester && go build -o ../../../bin/tester -v ./cmd/

.PHONY run:
run:
	./bin/arithmetical "127.0.0.1:8081" &> logarith.log
	./bin/tester -c "cfg/config.yml" &> logtester.log

.PHONY stop:
stop:
	killall -9 arithmetical
	killall -9 tester