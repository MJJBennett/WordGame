#ifndef WG_BOARD_HPP
#define WG_BOARD_HPP

#include <string>
#include "item.hpp"

namespace wg
{
class Board
{
public:
    wg::Table<Item> table_;
    void load_config(std::string filename);
};
}  // namespace wg

#endif  // WG_BOARD_HPP
