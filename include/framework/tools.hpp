#ifndef WG_TOOLS_HPP
#define WG_TOOLS_HPP

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

#endif  // WG_TOOLS_HPP
