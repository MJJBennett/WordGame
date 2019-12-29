#include "game/board.hpp"

#include <nlohmann/json.hpp>
#include "debug/log.hpp"
#include "framework/file_io.hpp"

using json = nlohmann::json;

void wg::Board::parse_board_update(const nlohmann::json& update)
{
    try
    {
        for (const auto& k : update.items())
        {
            const char row_id_char = k.key()[0];
            const int row_id       = row_id_char - 'A';
            wg::log::point(k.value().dump());
            const std::vector<int>& row = k.value().get<std::vector<int>>();
            for (size_t col_id = 0; col_id < row.size(); col_id++)
            {
                // This might be some of the ugliest framework code I've ever made
                // Rough times
                table_.at(col_id, row_id).character_ = decode(row.at(col_id));
            }
        }
    }
    catch (const json::type_error& e)
    {
        wg::log::err("[Board] JSON parsing error: ", e.what());
        wg::log::err("When trying to update JSON:\n", update.dump(2));
    }
}

void wg::Board::set_layout(const nlohmann::json& layout)
{
    try
    {
        for (const auto& k : layout.items())
        {
            const char row_id_char = k.key()[0];
            const int row_id       = row_id_char - 'A';
            wg::log::point(k.value().dump());
            const std::vector<int>& row = k.value().get<std::vector<int>>();
            for (size_t col_id = 0; col_id < row.size(); col_id++)
            {
                const unsigned char i = wg::multiplier::decode(
                    row.at(col_id));  // This number represents the multiplier
                table_.at(col_id, row_id).colour_      = multiplier_colour(i);
                table_.at(col_id, row_id).multipliers_ = i;
            }
        }
    }
    catch (const json::type_error& e)
    {
        wg::log::err("[Board] JSON parsing error: ", e.what());
        wg::log::err("When trying to update JSON:\n", layout.dump(2));
    }
}

sf::Color wg::Board::multiplier_colour(unsigned char multipliers)
{
    switch (multipliers)
    {
        case wg::multiplier::DoubleChar: return colours_[1];
        case wg::multiplier::DoubleWord: return colours_[2];
        case wg::multiplier::TripleChar: return colours_[3];
        case wg::multiplier::TripleWord: return colours_[4];
        case (wg::multiplier::DoubleWord & wg::multiplier::DoubleChar): return colours_[5];
        default: return colours_[0];
    }
}

bool wg::Board::parse_config(const nlohmann::json& config)
{
    wg::log::data("Using configuration", config.dump());

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
                    wg::log::point(">>> Loading string array for key: ", k.key());
                    set_config(k.key(), k.value().get<std::vector<std::string>>());
                }
                break;
            }
            case json::value_t::string:
            {
                wg::log::point(">>> Loading string for key: ", k.key());
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
        colours_[0] = wg::colour(value[0], value[1], value[2]);
    }
    else if (name == "doublechar-colour")
    {
        colours_[1] = wg::colour(value[0], value[1], value[2]);
    }
    else if (name == "doubleword-colour")
    {
        colours_[2] = wg::colour(value[0], value[1], value[2]);
    }
    else if (name == "triplechar-colour")
    {
        colours_[3] = wg::colour(value[0], value[1], value[2]);
    }
    else if (name == "tripleword-colour")
    {
        colours_[4] = wg::colour(value[0], value[1], value[2]);
    }
    else if (name == "doublechardoubleword-colour")
    {
        colours_[5] = wg::colour(value[0], value[1], value[2]);
    }
    return true;
}

bool wg::Board::set_config(std::string name, std::string value) { return true; }
