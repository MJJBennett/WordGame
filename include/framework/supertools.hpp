#ifndef WG_SUPERTOOLS_HPP
#define WG_SUPERTOOLS_HPP

#include <fstream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <random>
#include "debug/log.hpp"
#include "tools.hpp"

namespace wg
{
struct One
{
};

template <typename R>
auto select_randomly_destructively(R& r, One) -> char /*std::remove_reference<decltype(r.at(0))>*/
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, r.size() - 1);
    const auto pos = distribution(gen);
    const auto val = r[pos];
    r.erase(pos, 1);
    return val;
}

template <typename R>
auto select_randomly_destructively(R& r, size_t num = 0)
    -> std::vector<char /*std::remove_reference<decltype(r.at(0))>*/>
{
    if (num >= r.size()) return {};
    std::vector<char /*decltype(r.at(0))*/> vec;
    for (size_t i = 0; i < num; i++)
    {
        vec.push_back(select_randomly_destructively(r, One{}));
    }
    return vec;
}

/***
 * JSON helper code
 * If any of this is ever called more than once per session,
 * it probably needs a severe rework. For now, it'll cut down on
 * the size of some JSON loading code.
 */

enum class JSONError
{
    Open,
    Parse,
};
const std::string OpenError  = "WG_OPEN_ERROR";   // This may or may not be a hack
const std::string ParseError = "WG_PARSE_ERROR";  // But if it works for now, it works... for now
bool is_error(const nlohmann::json& jd, ::wg::JSONError error)
{
    switch (error)
    {
        case JSONError::Open: return jd.size() == 1 && jd.find(::wg::OpenError) != jd.end();
        case JSONError::Parse: return jd.size() == 1 && jd.find(::wg::ParseError) != jd.end();
    }
}

inline nlohmann::json filename_to_json(const std::string& fn)
{
    wg::log::point("Loading new layout from file: ", fn);

    std::ifstream input_file(fn); // Load this file
    if (!input_file.good())
    {
        wg::log::warn(__func__, ": Could not open file: ", fn);
        return {OpenError};
    }
    nlohmann::json data; // Into this object
    try
    {
        data = nlohmann::json::parse(input_file);
    }
    catch (const nlohmann::json::parse_error& e)
    {
        wg::log::warn(__func__, ": Could not parse json in file: ", fn);
        return {ParseError};
    }
    return data;
}
}  // namespace wg

#endif  // WG_SUPERTOOLS_HPP
