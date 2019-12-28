#ifndef WG_TABLE_HPP
#define WG_TABLE_HPP

#include <array>
#include <optional>
#include "assert.hpp"

namespace wg
{
template <typename CellType>
class Table
{
public:
    static constexpr unsigned int table_size = 15;

    std::array<float, 2> position_{0, 0};

public:
    const std::array<CellType, table_size>& get_column(unsigned int col_num) const
    {
        wg::assert_true(col_num < table_size);
        return items_[col_num];
    }

    CellType& at(unsigned int col_num, unsigned int row_pos) { return items_[col_num][row_pos]; }

    float get_width() const
    {
        if (!width_) calculate_width();
        return *width_;
    }
    float get_height() const
    {
        if (!height_) calculate_height();
        return *height_;
    }

    std::array<unsigned int, 2> get_tile_dimensions() const { return {tile_width, tile_height}; }
    std::array<unsigned int, 2> get_tile_offsets() const { return {offset_x, offset_y}; }
    std::array<float, 2> get_dimensions() const { return {get_width(), get_height()}; }

    void set_colour(unsigned int r, unsigned int g, unsigned int b)
    {
        for (auto&& col : items_)
        {
            for (auto&& i : col)
            {
                i.set_colour(r, g, b);
            }
        }
    }

private:
    void calculate_width() const { width_ = (float)table_size * (tile_width + offset_x); }
    void calculate_height() const { height_ = (float)table_size * (tile_height + offset_y); }

private:
    std::array<std::array<CellType, table_size>, table_size> items_;

    // We will cache these
    mutable std::optional<float> width_;
    mutable std::optional<float> height_;

    unsigned int tile_width  = 25;
    unsigned int tile_height = 25;
    unsigned int offset_y    = 3;
    unsigned int offset_x    = 3;
};
}  // namespace wg

#endif  // WG_TABLE_HPP
