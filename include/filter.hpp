#pragma once
#include <vector>
#include <optional>
#include <map>

#include "json.hpp"

namespace relay_middleware::nostr
{
    class NostrFilter
    {
    public:
        const std::optional<std::vector<std::string>> ids;
        const std::optional<std::vector<std::string>> authors;
        const std::optional<std::vector<int>> kinds;
        const std::map<std::string, std::vector<std::string>> tags;
        const std::optional<long> since;
        const std::optional<long> until;
        const std::optional<int> limit;
        const std::optional<std::string> search;

        static auto from_json(const nlohmann::json &json) -> NostrFilter
        {
            if (!json.is_object())
            {
                throw std::runtime_error("Filter json must be object");
            }
            auto ids = json.contains("ids") && json["ids"].is_array() ? std::optional<std::vector<std::string>>{json["ids"].get<std::vector<std::string>>()} : std::nullopt;
            auto authors = json.contains("authors") && json["authors"].is_array() ? std::optional<std::vector<std::string>>{json["authors"].get<std::vector<std::string>>()} : std::nullopt;
            auto kinds = json.contains("kinds") && json["kinds"].is_array() ? std::optional<std::vector<int>>{json["kinds"].get<std::vector<int>>()} : std::nullopt;
            auto since = json.contains("since") && json["since"].is_number() ? std::optional<long>{json["since"].get<long>()} : std::nullopt;
            auto until = json.contains("until") && json["until"].is_number() ? std::optional<long>{json["until"].get<long>()} : std::nullopt;
            auto limit = json.contains("limit") && json["limit"].is_number() ? std::optional<long>{json["limit"].get<long>()} : std::nullopt;
            auto search = json.contains("search") && json["search"].is_string() ? std::optional<std::string>(json["search"].get<std::string>()) : std::nullopt;

            auto tags = std::map<std::string, std::vector<std::string>>();
            for (auto &[k, v] : json.items())
            {
                if (v.is_array() && k.starts_with('#'))
                {
                    tags.emplace(k, v.get<std::vector<std::string>>());
                }
            }

            return NostrFilter{ids, authors, kinds, tags, since, until, limit, search};
        }

        auto to_json() const -> nlohmann::json
        {
            auto ret = nlohmann::json();

            if (ids.has_value())
            {
                ret["ids"] = ids.value();
            }
            if (authors.has_value())
            {
                ret["authors"] = authors.value();
            }
            if (kinds.has_value())
            {
                ret["kinds"] = kinds.value();
            }
            for (const auto &[k, v] : tags)
            {
                ret[k] = v;
            }
            if (since.has_value())
            {
                ret["since"] = since.value();
            }
            if (until.has_value())
            {
                ret["until"] = until.value();
            }
            if (limit.has_value())
            {
                ret["limit"] = limit.value();
            }
            if (search.has_value())
            {
                ret["search"] = search.value();
            }
            return ret;
        }
    };
}