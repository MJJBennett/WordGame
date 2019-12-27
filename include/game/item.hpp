#ifndef WG_ITEM_HPP
#define WG_ITEM_HPP

#include <SFML/Graphics/Color.hpp>
#include <optional>

#include "multipliers.hpp"

namespace wg
{
struct Item
{
    std::optional<char> character_;
    std::optional<MultiplierMask> multipliers_;
    sf::Color colour_{192, 87, 70};

    void set_colour(unsigned int r, unsigned int g, unsigned int b)
    {
        colour_ = sf::Color{(sf::Uint8)r, (sf::Uint8)g, (sf::Uint8)b};
    }
};
}  // namespace wg

#endif  // WG_ITEM_HPP
