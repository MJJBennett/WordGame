#ifndef WG_TABLE_HPP
#define WG_TABLE_HPP

#include <array>
#include <optional>

#include "game/item.hpp"

namespace wg
{

class Table
{
public:
    static constexpr unsigned int table_size = 15;

public:
    const std::array<Item, table_size>& get_column(unsigned int col_num) const
    {
        assert(col_num < table_size);
        return items_[col_num];
    }

private:
    std::array<std::array<Item, table_size>, table_size> items_;
};
}  // namespace wg

#endif  // WG_TABLE_HPP
