#pragma once

#include "HttpContext.h"
#include "HttpResponse.h"
#include "WebSocketContext.h"
#include "WebSocket.h"

#include "state.h"
#include "json.hpp"
#include "request.hpp"
#include "proxy.hpp"
#include "socket.hpp"

namespace uWS
{
    template <bool SSL>
    class NostrServer
    {
    private:
        uWS::HttpContext<SSL> *httpContext;

        static auto handleMessage(const std::string_view &message, const StateObj *state) -> void
        {
            try
            {
                auto j = nlohmann::json::parse(message);
                if (!j.is_array())
                {
                    return;
                }

                auto q = j[0].get<std::string>();
                if (q == "REQ")
                {
                    auto filter = NostrRequest::from_json(j);
                    state->proxy->push(std::move(filter));
                }
                else if (q == "EVENT")
                {
                    auto id = j[1]["id"].get<std::string>();
                    // do something
                    state->socket->ok(id, false, "not implemented");
                }
                else if (q == "CLOSE")
                {
                    // nothing
                }
                else
                {
                    state->socket->notice("Command not supported");
                }
            }
            catch (const std::exception &e)
            {
                state->socket->notice("Unknown error: " + std::string(e.what()));
            }
        }

    public:
        NostrServer()
        {
            us_socket_context_options_t options = {};
            httpContext = uWS::HttpContext<SSL>::create(uWS::Loop::get(), options);
        }

        auto ws(std::string path)
        {
            auto openHandler = [](auto *ws)
            {
                StateObj *state = ws->getUserData();
                std::cout << "New connection: " << ws->getRemoteAddressAsText() << std::endl;
                state->socket = new relay_middleware::nostr::NostrSocket(ws);
                state->proxy = new relay_middleware::proxy::RelayProxy("http://localhost:5167");
            };
            auto closeHandler = [](auto *ws, auto code, auto msg)
            {
                StateObj *state = ws->getUserData();
                delete state->proxy;
                delete state->socket;
                delete state;
            };
            auto messageHandler = [](auto *ws, std::string_view message, uWS::OpCode opCode)
            {
                std::cout << "In: " << message << std::endl;
                if (opCode == uWS::OpCode::TEXT)
                {
                    NostrServer::handleMessage(message, ws->getUserData());
                }
            };
            auto *webSocketContext = uWS::WebSocketContext<SSL, true, StateObj>::create(uWS::Loop::get(), (us_socket_context_t *)httpContext, nullptr);
            auto wsExt = (WebSocketContextData<SSL, StateObj> *)us_socket_context_ext(SSL, (us_socket_context_t *)webSocketContext);
            /* Copy all handlers */
            wsExt->openHandler = std::move(openHandler);
            wsExt->messageHandler = std::move(messageHandler);
            wsExt->drainHandler = std::move(nullptr);
            wsExt->subscriptionHandler = std::move(nullptr);
            wsExt->closeHandler = std::move(closeHandler);
            wsExt->pingHandler = std::move(nullptr);
            wsExt->pongHandler = std::move(nullptr);

            /* Copy settings */
            wsExt->maxPayloadLength = 128 * 1024;
            wsExt->maxBackpressure = 64 * 1024;
            wsExt->closeOnBackpressureLimit = true;
            wsExt->resetIdleTimeoutOnSend = true;
            wsExt->sendPingsAutomatically = true;
            wsExt->maxLifetime = 60;
            wsExt->compression = uWS::CompressOptions::DISABLED;

            /* Calculate idleTimeoutCompnents */
            wsExt->calculateIdleTimeoutCompnents(60);

            auto reqHandler = [webSocketContext](uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req) mutable
            {
                auto accept = req->getHeader("accept");
                if (accept == "application/nostr+json")
                {
                    res->write("{}");
                    res->end();
                    return;
                }
                else
                {
                    /* If we have this header set, it's a websocket */
                    auto secWebSocketKey = req->getHeader("sec-websocket-key");
                    if (secWebSocketKey.length() == 24)
                    {
                        /* Default handler upgrades to WebSocket */
                        std::string_view secWebSocketProtocol = req->getHeader("sec-websocket-protocol");
                        std::string_view secWebSocketExtensions = req->getHeader("sec-websocket-extensions");
                        res->template upgrade<StateObj>({}, secWebSocketKey, secWebSocketProtocol, secWebSocketExtensions, (struct us_socket_context_t *)webSocketContext);
                    }
                    else
                    {
                        /* Tell the router that we did not handle this request */
                        req->setYield(true);
                    }
                }
            };

            httpContext->onHttp("GET", path, std::move(reqHandler), true);
        }

        auto run(int port)
        {
            httpContext->listen(nullptr, port, 0);
            uWS::run();
        }
    };

    typedef NostrServer<false> App;
}