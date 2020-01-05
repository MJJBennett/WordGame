#ifndef WG_GAME_CONTEXT_HPP
#define WG_GAME_CONTEXT_HPP

#include <SFML/Graphics/RectangleShape.hpp>
#include <unordered_set>
#include "game/board.hpp"
#include "game/game_io.hpp"
#include "game/turn.hpp"
#include "game/update.hpp"
#include "wg_forward.hpp"

namespace wg
{
namespace GameOption
{
constexpr unsigned int None         = 0b0;
constexpr unsigned int TurnsEnabled = 0b1;
// constexpr unsigned int ? = 0b1 << 1;
}  // namespace GameOption

class GameContext
{
public:
    GameContext(wg::WindowContext& c, wg::ResourceManager& r, wg::UpdateHandler& u);

    void init();  // Called once

    // Everything we have that can be drawn, can receive input
    // We want to be able to manage that - so:
    void parse_input(sf::Event&);
    void maybe_command(const wg::Action& act);
    void send_playerlist();

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

    void print_debug();

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

    void do_turn(const std::string& player);
    void end_turn();

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
    std::unordered_map<std::string, int> players_;
    int my_score_{0};
    bool is_host_{false};

    sf::RectangleShape rect_;
    sf::Text legend_;

    std::optional<wg::Turn> turn_;

    sf::Text playerlist_;
    std::string playerlist_str_;
    void fix_playerlist()
    {
        playerlist_str_ = "Players:\n> " + io_.user_ + ": " + std::to_string(my_score_);
        for (auto&& [op, s] : players_)
        {
            playerlist_str_ += "\n- " + op + ": " + std::to_string(s);
        }
        playerlist_.setString(playerlist_str_);
    }

    bool hide_things_{false};
    bool just_entered_char_{false};
};
}  // namespace wg

#endif  // WG_GAME_CONTEXT_HPP
