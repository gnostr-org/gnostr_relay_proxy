#pragma once
#include "json.hpp"
#include "filter.hpp"

namespace relay_middleware::nostr
{
    class NostrRequest
    {
    public:
        const std::string id;
        const std::vector<NostrFilter> filters;

        static auto from_json(const nlohmann::json &data) -> NostrRequest
        {
            if (!data.is_array())
            {
                throw std::runtime_error("Data must be array");
            }
            if (data[0].get<std::string>() != "REQ")
            {
                throw std::runtime_error("Not REQ command");
            }
            auto filters = std::vector<NostrFilter>();
            auto f = data.begin();
            f += 2;
            for (; f != data.end(); f++)
            {
                auto fx = NostrFilter::from_json(*f);
                filters.push_back(fx);
            }
            return NostrRequest{
                data[1].get<std::string>(),
                filters,
            };
        }

        auto to_json() const -> nlohmann::json
        {
            auto ret = nlohmann::json();

            ret[0] = "REQ";
            ret[1] = id;
            for (const auto &x : filters)
            {
                ret.push_back(x.to_json());
            }
            return ret;
        }
    };
}