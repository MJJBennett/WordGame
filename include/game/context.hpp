#ifndef WG_GAME_CONTEXT_HPP
#define WG_GAME_CONTEXT_HPP

#include "game/item.hpp"
#include "framework/table.hpp"

namespace sf
{
    class Event;
}

namespace wg
{
class Renderer;

class GameContext
{
public:
    // A game has a table, a chat, a network context, and a few other things
    // Let's deal with the table first of all
    Table<Item> table_;

    // Everything we have that can be drawn, can receive input
    // We want to be able to manage that - so:
    void parse_input(sf::Event&);

    // We're going to add this for now as empty, but it'll come in handy
    // This runs once per game tick
    void update(){}

    // This is where we render everything!
    void render(wg::Renderer& renderer);

    bool running() { return running_; }

private:
    void parse_key_released(sf::Event&);
    void parse_text_entered(sf::Event&);
    void parse_mouse_released(sf::Event&);
    void parse_escape();
private:
    enum class Mode
    {
        Text,
        Default,
        SetTile,
    } mode_;

    std::optional<std::array<unsigned int, 2>> pending_tile_;

    bool running_{true};
};
}  // namespace wg

#endif  // WG_GAME_CONTEXT_HPP
