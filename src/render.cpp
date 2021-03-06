#include "framework/render.hpp"

#include "framework/resource.hpp"
#include "framework/resourcemanager.hpp"
#include "framework/table.hpp"
#include "framework/tools.hpp"
#include "framework/window_context.hpp"
#include "game/item.hpp"

#include <SFML/Graphics/Drawable.hpp>

wg::Renderer::Renderer(wg::WindowContext& window, wg::ResourceManager& manager)
    : window_(window), manager_(manager)
{
    rect_.setSize(sf::Vector2f{25, 25});
    rect_.setFillColor(sf::Color{128, 128, 128});
}

void wg::Renderer::render(const sf::Drawable& d) { window_.getTarget().draw(d); }

void wg::Renderer::render(const Table<Item>& i)
{
    // This is bad but I think I'm going to wait on fixing it
    // I'm hopeful that a better architecture will reveal itself in a dream
    const auto opts = wg::RenderOptions{};
    // This is beautiful, I've truly fallen in love
    const auto [width, height]      = i.get_tile_dimensions();
    const auto [offset_x, offset_y] = i.get_tile_offsets();
    const float move_x              = (float)(width + offset_x);
    const float move_y              = (float)(height + offset_y);

    // We'll draw the background first
    rect_.setFillColor(wg::colour(i.bg_r, i.bg_g, i.bg_b));
    rect_.setPosition(i.x, i.y);
    rect_.setSize(sf::Vector2f{move_x * i.table_size, move_y * i.table_size});
    window_.getTarget().draw(rect_);
    rect_.setSize(sf::Vector2f{25, 25});

    for (unsigned int col_num = 0; col_num < i.table_size; col_num++)
    {
        const auto& column = i.get_column(col_num);
        for (unsigned int row_num = 0; row_num < i.table_size; row_num++)
        {
            // So looking at this now I'm thinking that the overhead of x*y sf::RectangleShapes
            // in memory might be way better than running these methods x*y times per tick.
            // Pretty easy change to make once more implementation is done and it's actually
            // possible to test this hypothesis.
            const auto& item = column[row_num];
            const auto pos_x = i.x + col_num * move_x;
            const auto pos_y = i.y + row_num * move_y;

            // Colour
            rect_.setFillColor(item.colour_);

            // Background tile
            rect_.setPosition(sf::Vector2f{pos_x, pos_y});

            // Draw it
            window_.getTarget().draw(rect_);

            if (item.character_)
            {
                auto c = manager_.get({wg::ResourceType::text, std::string{*(item.character_)}});
                c->setPosition(pos_x + 4, pos_y - 4);
                window_.getTarget().draw(c->asDrawable());
            }
        }
    }
}
