#include "service.hpp"

#include "engine.hpp"

using namespace arithmetical;
using namespace service;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

namespace {


    void mulResultToMulResponse(MulResponse& aDst, const engine::Multiplier::Result& aSrc) {
        aDst.set_mul(aSrc.m_Val1);
        aDst.set_factor(aSrc.m_Val2);
        aDst.set_result(aSrc.m_Result);
        for (auto& sSrcExp : aSrc.m_IntermediateExps) {
            auto sExp = aDst.add_exps();
            sExp->set_act(static_cast<Exp_Action>(sSrcExp.m_Action));
            sExp->set_val1(sSrcExp.m_Val1);
            sExp->set_val2(sSrcExp.m_Val2);
            sExp->set_is_correction_step(sSrcExp.m_IsCorrectionStep);
            sExp->set_is_overflow(sSrcExp.m_IsOverflow);
        }
    }
}

Status ArithmeticalService::Mul(ServerContext *context, const MulRequest *request, MulResponse *reply) {
    if (request->grid_size() > 8) {
        return Status::CANCELLED;
    }

    engine::Multiplier sMultiplier(request->number_size());
    auto sMethod = request->method();
    engine::Multiplier::Result sRes;
    switch (sMethod) {
        case MulRequest_Method_CORRECTION_STEP_REVERSE: {
            details::ReverseBitset sMul(request->mul(), request->grid_size());
            details::ReverseBitset sFactor(request->factor(), request->grid_size());
            sRes = sMultiplier.mulWithCurrentStep(sMul, sFactor);
            break;
        }
        case MulRequest_Method_CORRECTION_STEP_ADDITIONAL: {
            details::AdditionalBitset sMul(request->mul(), request->grid_size());
            details::AdditionalBitset sFactor(request->factor(), request->grid_size());
            sRes = sMultiplier.mulWithCurrentStep(sMul, sFactor);
            break;
        }
        case MulRequest_Method_ANALYSIS_ADJACENT_BITS: {
            details::AdditionalBitset sMul(request->mul(), request->grid_size());
            details::AdditionalBitset sFactor(request->factor(), request->grid_size());
            sRes = sMultiplier.mulWithAnalysisAdjacentBits(sMul, sFactor);
            break;
        }
        case MulRequest_Method_INVERT_NUMBER_BITS: {
            details::ReverseBitset sMul(request->mul(), request->grid_size());
            details::ReverseBitset sFactor(request->factor(), request->grid_size());
            sRes = sMultiplier.mulWithInvertNumberBits(sMul, sFactor);
            break;
        }
        case MulRequest_Method_MUL_ON_2_FACTOR_BITS: {
                    details::DirectBitset sMul(request->mul(), request->grid_size());
                    details::DirectBitset sFactor(request->factor(), request->grid_size());
                    sRes = sMultiplier.mulOnTwoFactorBits(sMul, sFactor);
                    break;
        }
        case MulRequest_Method_MulRequest_Method_INT_MIN_SENTINEL_DO_NOT_USE_:
        case MulRequest_Method_MulRequest_Method_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }
    mulResultToMulResponse(*reply, sRes);
    return Status::OK;
}

grpc::Status
ArithmeticalService::Code(::grpc::ServerContext *context, const ::CodeRequest *request, ::CodeResponse *response) {
    details::DirectBitset sDirect(request->num(), 7);
    details::ReverseBitset sReverse(request->num(), 7);
    details::AdditionalBitset sAdditional(request->num(), 7);
    response->set_direct(sDirect.toString());
    response->set_reverse(sReverse.toString());
    response->set_additional(sAdditional.toString());
    return Status::OK;
}

grpc::Status
ArithmeticalService::MulOn2(::grpc::ServerContext *context, const ::MulOn2Request *request, ::CodeResponse *response) {
    details::DirectBitset sDirect(request->mul(), 7);
    details::ReverseBitset sReverse(request->mul(), 7);
    details::AdditionalBitset sAdditional(request->mul(), 7);
    auto sPow = request->pow();
    if (sPow < 0) {
        sDirect >>= -sPow;
        sReverse >>= -sPow;
        sAdditional >>= -sPow;
    } else {
        sDirect <<= sPow;
        sReverse <<= sPow;
        sAdditional <<= sPow;
    }
    response->set_direct(sDirect.toString());
    response->set_reverse(sReverse.toString());
    response->set_additional(sAdditional.toString());
    return Status::OK;
}

void arithmetical::service::runService(std::string & aAddr) {

    ArithmeticalService service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(aAddr, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}
