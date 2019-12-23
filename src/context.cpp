#include "game/context.hpp"

#include <SFML/Window/Event.hpp>
#include "framework/render.hpp"

#ifdef _WIN32
#include <assert.h>
#endif

void wg::GameContext::parse_input(sf::Event& e)
{
    switch (e.type)
    {
        case sf::Event::KeyReleased:
            if (e.key.code == sf::Keyboard::Escape)
            {
                parse_escape();
            }
            else if (mode_ == Mode::Default)
                parse_key_released(e);
            break;
        case sf::Event::TextEntered:
            if (mode_ != Mode::Default) parse_text_entered(e);
        case sf::Event::MouseButtonReleased: parse_mouse_released(e); break;
        default: break;
    }
}

void wg::GameContext::render(wg::Renderer& renderer) { renderer.render(table_); }

// Event handling
void wg::GameContext::parse_key_released(sf::Event& e) {}
void wg::GameContext::parse_text_entered(sf::Event& e)
{
    if (mode_ == Mode::SetTile)
    {
        const auto [col, row] = *pending_tile_;
        const auto c          = (char)e.text.unicode;
        set_tile(col, row, c);
        mode_       = Mode::Default;
        last_update = GameUpdate{int(col), int(row), c};
    }
}

void wg::GameContext::set_tile(int col, int row, char c)
{
    auto& item      = table_.at(col, row);
    item.character_ = c;
}

void wg::GameContext::parse_mouse_released(sf::Event& e)
{
    const auto x              = e.mouseButton.x;
    const auto y              = e.mouseButton.y;
    const auto [tw, th]       = table_.get_dimensions();
    const auto [tx, ty]       = table_.position_;
    const auto [tilew, tileh] = table_.get_tile_dimensions();
    const auto [offx, offy]   = table_.get_tile_offsets();

    if (x > tx && x < tx + tw && y > ty && y < ty + th)
    {
        // Okay, we need to figure out which row we're clicking in:
        const unsigned int row = static_cast<int>((x - tx) / (tilew + offx));
        const unsigned int col = static_cast<int>((y - ty) / (tileh + offy));
        // TODO - This should all be cached 100%, I think
        // But only if this is a noticeable performance hit, unlikely

        assert(row < table_.table_size && row >= 0 && col < table_.table_size && col >= 0);

        pending_tile_ = {row, col};
        mode_         = Mode::SetTile;
    }
}
void wg::GameContext::parse_escape() { running_ = false; }
