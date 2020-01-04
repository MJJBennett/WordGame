#ifndef WG_BOARD_HPP
#define WG_BOARD_HPP

#include <array>
#include <nlohmann/json.hpp>  // TODO - should be json_fwd.hpp, which seems to be missing
#include <string>
#include <vector>
#include "framework/table.hpp"
#include "game/scoring.hpp"
#include "item.hpp"
#include "tile.hpp"

namespace wg
{
class Board
{
public:
    wg::Table<Item> table_;

    void parse_board_update(const nlohmann::json&);
    void set_layout(const nlohmann::json&);

    bool parse_config(const nlohmann::json&);

    bool set_config(std::string name, std::vector<std::string> value);
    bool set_config(std::string name, std::vector<unsigned int> value);
    bool set_config(std::string name, std::string value);

    // Converts '0' to 0, '1' to 1, etc
    static int encode(char board_char) { return board_char - '0'; }
    // Converts 0 to '0', 1 to '1', etc
    static char decode(int board_num) { return board_num + '0'; }

    int score(const std::vector<Tile>&) const;
    wg::ScoreMap scores_;

    sf::Color multiplier_colour(unsigned char multipliers) const;
    std::array<sf::Color, 8> colours_;
};
}  // namespace wg

#endif  // WG_BOARD_HPP
