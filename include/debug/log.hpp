#ifndef WG_DEBUG_HPP
#define WG_DEBUG_HPP

#include <iostream>

namespace wg::log
{
template <typename T>
void _log(T t)
{
    std::cout << t << std::endl;
}
template <typename T, typename... Ts>
void _log(T t, Ts... ts)
{
    std::cout << t;
    _log(std::forward<Ts>(ts)...);
}

/** Logs things inside of quotes. **/
template <typename T>
void quoted(T t)
{
    _log('"', t, '"');
}

template <typename T, typename... Ts>
void quoted(T t, Ts... ts)
{
    _log('"', t, std::forward<Ts>(ts)..., '"');
}

/** Logs the contents of a range. **/
template <typename R>
void range(const R& r)
{
    for (auto&& c : r) std::cout << c << ", ";
    std::cout << std::endl;
}

template <typename T>
void _err(T t)
{
    std::cerr << t << std::endl;
}

template <typename T, typename... Ts>
void _err(T t, Ts... ts)
{
    std::cerr << t;
    _err(std::forward<Ts>(ts)...);
}

/** Logs an error message. **/
template <typename... Ts>
void err(Ts... ts)
{
    _err("Error: ", std::forward<Ts>(ts)...);
}

/** Logs a warning message. **/
template <typename... Ts>
void warn(Ts... ts)
{
    _log("Warning: ", std::forward<Ts>(ts)...);
}

/** Logs a message for a specific line of code. **/
template <typename... Ts>
void point(Ts... ts)
{
    _log("> ", std::forward<Ts>(ts)...);
}

/** Logs printable data with a prepended label. **/
template <typename L, typename... Ts>
void data(const L& label, Ts... ts)
{
    _log(">> DATA: ", label, std::forward<Ts>(ts)..., "\n------------------");
}

/** For logging the value of a variable. **/
template <typename K, typename V>
void value(const K& name, const V& value)
{
    _log(">>> The value of ", name, " is ", value);
}
}  // namespace wg::log

#endif  // WG_DEBUG_HPP
