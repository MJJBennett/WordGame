#ifndef WG_SUPERTOOLS_HPP
#define WG_SUPERTOOLS_HPP

#include <iterator>
#include <random>
#include "tools.hpp"

namespace wg
{
struct One
{
};

template <typename R>
auto select_randomly_destructively(R& r, One) -> char/*std::remove_reference<decltype(r.at(0))>*/
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
auto select_randomly_destructively(R& r, size_t num = 0) -> std::vector<char /*std::remove_reference<decltype(r.at(0))>*/>
{
    if (num >= r.size()) return {};
    std::vector<char/*decltype(r.at(0))*/> vec;
    for (size_t i = 0; i < num; i++)
    {
        vec.push_back(select_randomly_destructively(r, One{}));
    }
    return vec;
}
}  // namespace wg

#endif  // WG_SUPERTOOLS_HPP
