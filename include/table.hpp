#ifndef WG_TABLE_HPP
#define WG_TABLE_HPP

#include <array>
#include <optional>

#include "item.hpp"

namespace wg
{
// Forward declarations
struct RenderOptions;

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

    virtual bool has_render_opts() const noexcept;
    virtual const RenderOptions& get_render_opts() const;

private:
    std::array<std::array<Item, table_size>, table_size> items_;
};
}  // namespace wg

#endif  // WG_TABLE_HPP
