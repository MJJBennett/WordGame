#ifndef WG_BOARD_HPP
#define WG_BOARD_HPP

#include <nlohmann/json.hpp>  // TODO - should be json_fwd.hpp, which seems to be missing
#include <string>
#include <vector>
#include "framework/table.hpp"
#include "item.hpp"

namespace wg
{
class Board
{
public:
    wg::Table<Item> table_;
    bool parse_config(const nlohmann::json&);

    bool set_config(std::string name, std::vector<std::string> value);
    bool set_config(std::string name, std::vector<unsigned int> value);
    bool set_config(std::string name, std::string value);
};
}  // namespace wg

#endif  // WG_BOARD_HPP
