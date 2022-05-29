#pragma once
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "arithmetical.grpc.pb.h"



namespace arithmetical::service {
    class ArithmeticalService final : public Arithmetical::Service {
        grpc::Status Mul(grpc::ServerContext *context, const MulRequest *request, MulResponse *reply) override;
        grpc::Status Code(::grpc::ServerContext* context, const ::CodeRequest* request, ::CodeResponse* response) override;
    };

    void runService(std::string& aAddr);
}