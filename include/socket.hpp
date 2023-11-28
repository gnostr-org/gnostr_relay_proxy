#pragma once
#include <sstream>

#include <WebSocket.h>
#include "state.h"

namespace relay_middleware::nostr
{
    class NostrSocket
    {
    private:
        uWS::WebSocket<false, true, StateObj> *ws;

    public:
        NostrSocket(uWS::WebSocket<false, true, StateObj> *ws)
        {
            this->ws = ws;
        }

        auto eose(const std::string &id) const -> void
        {
            auto msg = "[\"EOSE\",\"" + id + "\"]";
            ws->send(msg, uWS::OpCode::TEXT);
        }

        auto notice(const std::string &msg) const -> void
        {
            auto notice = "[\"NOTICE\",\"" + msg + "\"]";
            ws->send(notice, uWS::OpCode::TEXT);
        }

        auto ok(const std::string &id, bool accepted, const std::string &reason) const -> void
        {
            auto msg = "[\"OK\",\"" + id + "\"," + (accepted ? "true" : "false") + ",\"" + reason + "\"]";
            ws->send(msg, uWS::OpCode::TEXT);
        }
    };
}