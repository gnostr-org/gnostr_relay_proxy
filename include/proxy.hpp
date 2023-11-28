#pragma once
#include <queue>
#include <grpcpp/grpcpp.h>

#include "request.hpp"
#include "api.hpp"

using namespace relay_middleware::nostr;

namespace relay_middleware::proxy
{
    class RelayProxy
    {
    private:
        std::queue<const NostrRequest> req_queue;
        RelayHandlerClient client;

    public:
        RelayProxy(const std::string &backend) : client(grpc::CreateChannel(backend, grpc::InsecureChannelCredentials()))
        {
        }

        auto push(const NostrRequest &&req) -> void
        {
            req_queue.push(std::move(req));
        }

        auto next() -> void
        {
            auto req = req_queue.back();
        }
    };
}