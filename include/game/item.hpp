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
    sf::Color colour_{198, 178, 188};
};
}  // namespace wg

#endif  // WG_ITEM_HPP
