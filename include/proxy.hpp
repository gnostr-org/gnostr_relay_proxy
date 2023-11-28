#pragma once
#include <queue>

#include "request.hpp"

using namespace relay_middleware::nostr;

namespace relay_middleware::proxy
{
    class RelayProxy
    {
    private:
        std::queue<const NostrRequest> req_queue;

    public:
        auto push(const NostrRequest &&req) -> void
        {
            req_queue.push(std::move(req));
        }
    };
}