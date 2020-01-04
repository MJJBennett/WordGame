#ifndef WG_TILE_HPP
#define WG_TILE_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <optional>
#include "game/boardcoordinate.hpp"

namespace wg
{
struct Tile : public sf::Drawable
{
public:
    Tile(char letter) : letter_(letter) {}
    std::optional<BoardCoordinate> board_pos_;
    char letter_;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
}  // namespace wg

#endif  // WG_TILE_HPP
