#ifndef WG_TILE_HPP
#define WG_TILE_HPP

#include <SFML/Graphics/RectangleShape.hpp>
#include <optional>
#include "game/boardcoordinate.hpp"
#include "game/multipliers.hpp"

namespace wg
{
struct Tile : public sf::RectangleShape
{
public:
    Tile(char letter) : letter_(letter) {}
    std::optional<BoardCoordinate> board_pos_;
    char letter_;
    wg::MultiplierMask multipliers_;

    bool operator==(const Tile& other) const noexcept
    {
        return (other.board_pos_ && board_pos_ && *(other.board_pos_) == *board_pos_ &&
                other.letter_ == letter_);
    }
};
}  // namespace wg

#endif  // WG_TILE_HPP
