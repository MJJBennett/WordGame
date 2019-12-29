#ifndef WG_GAME_CONTEXT_HPP
#define WG_GAME_CONTEXT_HPP

#include "game/board.hpp"
#include "game/game_io.hpp"
#include "game/update.hpp"
#include "wg_forward.hpp"
#include <unordered_set>

namespace wg
{
namespace GameOption
{
constexpr unsigned int None = 0b0;
constexpr unsigned int TurnsEnabled = 0b1;
//constexpr unsigned int ? = 0b1 << 1;
}

class GameContext
{
public:
    GameContext(wg::WindowContext& c, wg::ResourceManager& r, wg::UpdateHandler& u);

    void init(); // Called once

    // Everything we have that can be drawn, can receive input
    // We want to be able to manage that - so:
    void parse_input(sf::Event&);

    // This runs once per game tick
    void update();

    // This is where we render everything!
    void render(wg::Renderer& renderer);

    bool running() { return running_; }

    void set_tile(int col, int row, char c);

    bool load_config(std::string filename);
    bool parse_config(const nlohmann::json& config);
    bool set_config(std::string name, std::vector<std::string> value);
    bool set_config(std::string name, std::vector<unsigned int> value);
    bool set_config(std::string name, std::string value);

public:
    // A game has a table, a chat, a network context, and a few other things
    // Let's deal with the table first of all
    wg::Board board_;

    // We set the background here, the application takes it for us.
    sf::Color background_{208, 227, 196};

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
    } mode_ = Mode::Default;

    std::optional<std::array<unsigned int, 2>> pending_tile_;
    wg::UpdateHandler& update_handler;
    bool running_{true};
    GameIO io_;

    // This is game state stuff, until we refactor it out
    std::unordered_set<std::string> players_;
    bool is_host_{false};
};
}  // namespace wg

#endif  // WG_GAME_CONTEXT_HPP
