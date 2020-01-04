#ifndef WG_TILE_HPP
#define WG_TILE_HPP

#include <SFML/Graphics/RectangleShape.hpp>
#include <optional>
#include <string>
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

inline std::string to_string(const Tile& t)
{
    const auto p = t.board_pos_;
    std::string r = "Tile(" + std::string{t.letter_} + ")";
    if (p) r += " @ " + std::to_string(p->x_) + ", " + std::to_string(p->y_);
    r += " x " + wg::multiplier::to_string(t.multipliers_.multipliers_);
    return r;
}
}  // namespace wg

#endif  // WG_TILE_HPP
