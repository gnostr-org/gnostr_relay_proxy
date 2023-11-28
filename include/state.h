#ifndef __STATE_HPP
#define __STATE_HPP

namespace relay_middleware
{
	namespace nostr
	{
		class NostrSocket;
	}
	namespace proxy
	{
		class RelayProxy;
	}
}

typedef struct StateObj
{
	relay_middleware::proxy::RelayProxy *proxy;
	relay_middleware::nostr::NostrSocket *socket;
} StateObj;

#endif