#include "framework/window_item.hpp"

#include "framework/window_io.hpp"
#include "assert.hpp"

wg::Button::Button(std::string text, const sf::Font& font, float x, float y, float w, float h)
    : x_(x), y_(y), w_(w), h_(h)
{
    text_.setFont(font);
    text_.setString(text);
    text_.setCharacterSize(28);
    text_.setFillColor(sf::Color::Black);
    text_.setPosition(x_, y_);

    rect_.setSize(sf::Vector2f{w, h});
    rect_.setFillColor(sf::Color{198, 198, 198});
    rect_.setPosition(x_, y_);

    shade_.setSize(sf::Vector2f{w, h});
    shade_.setFillColor(sf::Color{128, 128, 128, 128});
    shade_.setPosition(x_, y_);
}

bool wg::Button::accept_click(const sf::Event& e)
{
    wg::abort_if(e.type == sf::Event::MouseButtonPressed);

    if (is_inside(e.mouseButton.x, e.mouseButton.y)) clicked_ = true;
    return clicked_;
}

bool wg::Button::accept_release(const sf::Event& e)
{
    wg::abort_if(e.type == sf::Event::MouseButtonReleased);

    bool full_click = clicked_ && is_inside(e.mouseButton.x, e.mouseButton.y);
    reset();

    return full_click;
}

bool wg::Button::is_inside(int x, int y)
{
    if ((x > x_) && (x < x_ + w_) && (y > y_) && (y < y_ + h_)) return true;
    return false;
}

void wg::Button::set_x(float x)
{
    x_ = x;
    update_position();
}

void wg::Button::update_position()
{
    const auto text_width = text_.getGlobalBounds().width;
    text_.setPosition(wg::window_io::center(w_, text_width) + x_, y_);
    rect_.setPosition(x_, y_);
    shade_.setPosition(x_, y_);
}
