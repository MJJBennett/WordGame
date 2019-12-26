#ifndef WG_TOOLS_HPP
#define WG_TOOLS_HPP

#include <array>

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
}  // namespace wg

#endif  // WG_TOOLS_HPP
