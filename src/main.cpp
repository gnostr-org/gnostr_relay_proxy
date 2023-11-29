#include "server.hpp"

int main()
{
	constexpr auto port = 3334;

	auto server = new uWS::App();
	server->ws("/");
	server->run(port);

	std::cout << "Failed to listen on port {port}" << std::endl;
}