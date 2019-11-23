#ifndef WG_TABLE_HPP
#define WG_TABLE_HPP

#include <array>
#include <optional>

#include "multipliers.hpp"

namespace wg
{
struct Item
{
    std::optional<char> character_;
    std::optional<MultiplierMask> multipliers_;
};

class Table
{
public:
private:
    std::array<std::array<Item, 15>, 15> items_;
};
}  // namespace wg

#endif  // WG_TABLE_HPP
