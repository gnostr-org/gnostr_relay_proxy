#pragma once

#include "json.hpp"

namespace relay_middelware::nostr {
    class NostrEvent {
        public:
        const std::string id;
        const std::string pubkey;
        const int kind;
        const long created_at;
        const std::vector<std::vector<std::string>> tags;
        const std::string sig;

        static auto from_json(const nlohmann::json &data) -> NostrEvent {

        }
    };
}