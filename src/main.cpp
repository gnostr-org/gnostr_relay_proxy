#include <sstream>

#include "App.h"
#include "json.hpp"
#include "request.hpp"
#include "proxy.hpp"
#include "socket.hpp"
#include "state.h"

int main()
{
	uWS::App().ws<StateObj>("/", {.compression = uWS::SHARED_COMPRESSOR,
								  .maxPayloadLength = 16 * 1024 * 1024,
								  .idleTimeout = 16,
								  .maxBackpressure = 1 * 1024 * 1024,
								  .closeOnBackpressureLimit = false,
								  .resetIdleTimeoutOnSend = false,
								  .sendPingsAutomatically = true,
								  .upgrade = nullptr,
								  .open = [](auto *ws)
								  {
									StateObj *state = ws->getUserData();
									std::cout << "New connection" << std::endl;
									state->socket = new relay_middleware::nostr::NostrSocket(ws);
									state->proxy = new relay_middleware::proxy::RelayProxy(); },
								  .close = [](auto *ws, auto code, auto msg)
								  {
										StateObj *state = ws->getUserData();
										delete state->proxy;
										delete state->socket; },
								  .message = [](auto *ws, std::string_view message, uWS::OpCode opCode)
								  {
									  std::cout << "In: " << message << std::endl;
									  if (opCode == uWS::OpCode::TEXT)
									  {
										  StateObj *state = ws->getUserData();
										try {

										  auto j = nlohmann::json::parse(message);
										  if (!j.is_array())
										  {
											  return;
										  }

										  auto q = j[0].get<std::string>();
										  if (q == "REQ")
										  {
											  auto filter = NostrRequest::from_json(j);
											  std::cout << "Out: " << filter.to_json() << std::endl;

											  state->socket->eose(filter.id);
										  }
										  else if (q == "EVENT")
										  {
												auto id = j[1]["id"].get<std::string>();
											  // do something
											  state->socket->ok(id, false, "not implemented");
										  } else if (q == "CLOSE") {
											// nothing
										  } else {
											state->socket->notice("Command not supported");
										  }
										}catch (const std::exception& e) {
											state->socket->notice("Unknown error: " + std::string(e.what()));
										}
									  } }})
		.listen(3334, [](auto *listen_socket)
				{
	    if (listen_socket) {
			std::cout << "Listening on port " << 3334 << std::endl;
	    } })
		.run();

	std::cout << "Failed to listen on port 3334" << std::endl;
}