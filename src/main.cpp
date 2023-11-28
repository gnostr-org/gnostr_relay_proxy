#include <sstream>

#include "App.h"
#include "json.hpp"
#include "request.hpp"
#include "proxy.hpp"
#include "socket.hpp"
#include "state.h"

void handleMessage(const std::string_view &message, const StateObj *state)
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

int main()
{
	auto openHandler = [](auto *ws)
	{
		StateObj *state = ws->getUserData();
		std::cout << "New connection" << std::endl;
		state->socket = new relay_middleware::nostr::NostrSocket(ws);
		state->proxy = new relay_middleware::proxy::RelayProxy("http://localhost:5167");
	};
	auto closeHandler = [](auto *ws, auto code, auto msg)
	{
		StateObj *state = ws->getUserData();
		delete state->proxy;
		delete state->socket;
	};
	auto messageHandler = [](auto *ws, std::string_view message, uWS::OpCode opCode)
	{
		std::cout << "In: " << message << std::endl;
		if (opCode == uWS::OpCode::TEXT)
		{
			handleMessage(message, ws->getUserData());
		}
	};
	auto listenHandler = [](auto *listen_socket)
	{
		if (listen_socket)
		{
			std::cout << "Listening on port " << 3334 << std::endl;
		}
	};

	uWS::App::WebSocketBehavior<StateObj> params = {};
	params.open = openHandler;
	params.close = closeHandler;
	params.message = messageHandler;

	uWS::App().ws<StateObj>("/", std::move(params)).listen(3334, listenHandler).run();

	std::cout << "Failed to listen on port 3334" << std::endl;
}