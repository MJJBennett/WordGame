#include "framework/render.hpp"

#include "game/item.hpp"
#include "framework/table.hpp"
#include "framework/tools.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

wg::Renderer::Renderer(sf::RenderWindow& window) : window_(window)
{
    rect_.setSize(sf::Vector2f{25, 25});
    rect_.setFillColor(sf::Color{128, 128, 128});
}

void wg::Renderer::render(const sf::Drawable& d) { window_.draw(d); }

void wg::Renderer::render(const Item& i)
{
    rect_.setPosition(sf::Vector2f{offset_ * 26.f, position_ * 26.f});
    window_.draw(rect_);
    position_++;
}

void wg::Renderer::render(const Table& i)
{
    const auto opts = wg::RenderOptions{};
    const int width  = default_val(opts.value("width"), 25);
    const int height = default_val(opts.value("height"), 25);
    const float offset_x = width + default_val(opts.value("x-offset"), 3);
    const float offset_y = height + default_val(opts.value("y-offset"), 3);

    for (unsigned int col_num = 0; col_num < i.table_size; col_num++)
    {
        const auto& column = i.get_column(col_num);
        for (unsigned int row_num = 0; row_num < i.table_size; row_num++)
        {
            // So looking at this now I'm thinking that the overhead of x*y sf::RectangleShapes
            // in memory might be way better than running these methods x*y times per tick.
            // Pretty easy change to make once more implementation is done and it's actually possible
            // to test this hypothesis.
            rect_.setFillColor(column[row_num].colour_);
            rect_.setPosition(sf::Vector2f{col_num * static_cast<float>(offset_x),
                                           row_num * static_cast<float>(offset_y)});
            window_.draw(rect_);
        }
    }
}

void wg::Renderer::start_render(unsigned int offset)
{
    position_ = 0;
    offset_   = offset;
}
