#ifndef WG_GAME_TURN_HPP
#define WG_GAME_TURN_HPP

#include <string>
#include <vector>
#include "tile.hpp"

namespace wg
{
struct Turn
{
    std::string player_;
    std::vector<Tile> letters_;
};
}  // namespace wg

#endif  // WG_GAME_TURN_HPP
