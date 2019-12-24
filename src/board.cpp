#include "game/board.hpp"

#include <nlohmann/json.hpp>
#include "debug/log.hpp"
#include "framework/file_io.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool wg::Board::parse_config(const nlohmann::json& config)
{
    wg::log::data("Using configuration", config.dump(2));

    // So we can set things like tile colour, etc
    for (const auto& k : config.items())
    {
        switch (k.value().type())
        {
            case json::value_t::array:
            {
                json arr = k.value().get<json>();
                if (arr.size() == 0) continue;
                if (arr[0].type() == json::value_t::number_unsigned)
                {
                    wg::log::point(">>> Loading unsigned array for key: ", k.key());
                    set_config(k.key(), k.value().get<std::vector<unsigned int>>());
                }
                else
                {
                    wg::log::point(">>> Loading string array for key: ", k);
                    set_config(k.key(), k.value().get<std::vector<std::string>>());
                }
                break;
            }
            case json::value_t::string:
            {
                wg::log::point(">>> Loading string for key: ", k);
                set_config(k.key(), k.value().get<std::string>());
                break;
            }
            default:
                wg::log::warn(__func__, ": Tried to load unsupported JSON: ", k.value().dump(1));
                break;
        }
    }

    return true;
}

bool wg::Board::set_config(std::string name, std::vector<std::string> value) { return true; }

bool wg::Board::set_config(std::string name, std::vector<unsigned int> value)
{
    if (name == "tile-colour")
    {
        table_.set_colour(value[0], value[1], value[2]);
    }
    if (name == "background-colour")
    {
        table_.set_colour(value[0], value[1], value[2]);
    }
    return true;
}

bool wg::Board::set_config(std::string name, std::string value) { return true; }
