#include "game/scoring.hpp"

#include "debug/log.hpp"
#include "framework/json_tools.hpp"
#include "framework/supertools.hpp"

void wg::ScoreMap::from_file(const std::string& filename)
{
    auto data = wg::filename_to_json(filename);
    if (wg::is_error(data, wg::JSONError::Open))
    {
        wg::log::err("Could not open file: ", filename);  // Error is the author, hm
        return;
    }
    if (wg::is_error(data, wg::JSONError::Parse))
    {
        wg::log::err("Could not parse json in file: ", filename);  // Oh well
        return;
    }

    from_json(data);
}

void wg::ScoreMap::from_string(const std::string& str)
{
    if (const auto data = wg::try_parse(str); data)
    {
        from_json(*data);
    }
    else
    {
        wg::log::err("Could not parse json in string: ", str);  // Oh well
        return;
    }
}

void wg::ScoreMap::from_json(const nlohmann::json& data)
{
    for (const auto& k : data.items())
    {
        const std::string& key = k.key();
        const int value        = k.value();

        if (key.size() != 1) wg::log::warn(__func__, ": Converting string to character: ", key);

        map_.insert({key[0], value});
    }
}
