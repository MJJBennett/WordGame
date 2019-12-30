#ifndef WG_JSON_IO_HPP
#define WG_JSON_IO_HPP

#include <nlohmann/json.hpp>
#include <optional>
#include "debug/log.hpp"
#include "file_io.hpp"

namespace wg
{
inline std::optional<nlohmann::json> try_parse(const std::string& str)
{
    try
    {
        return nlohmann::json::parse(str);
    }
    catch (const nlohmann::json::parse_error& e)
    {
        wg::log::warn("Could not parse JSON: ", str);
        return {};
    }
}
}  // namespace wg

#endif  // WG_JSON_IO_HPP
