#ifndef WG_TOOLS_HPP
#define WG_TOOLS_HPP

#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <array>
#include <string>
#include <vector>
#include "debug/log.hpp"

namespace wg
{
template <typename T, typename R>
bool in(T t, const R& r)
{
    // Note to self:
    // This could probably be implemented as a series of
    // overloads, conditional on the behaviour of the range.
    // e.g. .find(T) should always be preferred over this
    // range-based search (as it should be better or the same
    // in almost any conceivable situation)
    for (auto&& t2 : r)
        if (t == t2) return true;
    return false;
}

template <typename R>
bool has_chars(R r)
{
    std::array<char, 3> non_chars = {' ', '\r', '\n'};
    for (auto&& c : r)
    {
        if (in(c, non_chars)) continue;
        return true;
    }
    return false;
}

template <typename Optional, typename Value>
auto default_val(Optional o, Value def) -> Value
{
    return o ? *o : def;
}

template <typename Optional>
auto get_char(unsigned int c) -> Optional
{
    return (c < 128 && c > 31) ? (Optional{(char)c}) : Optional{};
}

inline bool startswith(const std::string& str, const std::string& start)
{
    return ((start.size() <= str.size()) && str.substr(0, start.size()) == start);
}

inline auto get_arg(const std::string& str) -> std::string
{
    const auto it = str.find("=");
    if (it == std::string::npos) return "";
    return str.substr(it + 1, str.size());
};

inline sf::Color colour(unsigned int r, unsigned int g, unsigned int b)
{
    return {(sf::Uint8)r, (sf::Uint8)g, (sf::Uint8)b};
}

inline std::vector<std::string> split(const std::string& str, char delim)
{
    std::vector<std::string> v;
    auto itr = str.begin();
    while (itr != str.end())
    {
        itr = std::find_if_not(itr, str.end(), [delim](char c) { return c == delim; });
        const auto itr_end = std::find_if(itr, str.end(), [delim](char c) { return c == delim; });
        v.push_back({itr, itr_end});
        itr = itr_end;
    }
    return v;
}

template <typename t>
inline std::string encode_range(const t& r)
{
    std::string s{};
    for (auto&& v : r)
    {
        s += v + '|';
    }
    if (s.size()) s.pop_back();
    return s;
}

inline std::vector<std::string> decode_range(const std::string& r) { return ::wg::split(r, '|'); }

inline int atoi_default(const std::string& str, int dv = 0)
{
    try
    {
        return std::stoi(str);
    }
    catch (const std::invalid_argument& e)
    {
        wg::log::err_ignored(e.what());
    }
}
}  // namespace wg

#endif  // WG_TOOLS_HPP
