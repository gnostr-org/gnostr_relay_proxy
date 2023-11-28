#pragma once
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include "api.grpc.pb.h"

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

namespace relay_middleware
{
    class RelayHandlerClient
    {
    public:
        RelayHandlerClient(std::shared_ptr<Channel> channel) : stub_(RelayHandler::NewStub(channel)) {}

    private:
        std::unique_ptr<RelayHandler::Stub> stub_;
        CompletionQueue cq_;
    };
}