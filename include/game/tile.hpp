#ifndef WG_TILE_HPP
#define WG_TILE_HPP

#include <SFML/Graphics/Drawable.hpp>

namespace wg
{
class Tile : public sf::Drawable
{
public:
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
}
}  // namespace wg

#endif  // WG_TILE_HPP
