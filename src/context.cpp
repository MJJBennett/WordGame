#include "game/context.hpp"

#include <SFML/Window/Event.hpp>
#include <nlohmann/json.hpp>
#include "assert.hpp"
#include "commands.hpp"
#include "debug/log.hpp"
#include "framework/file_io.hpp"
#include "framework/json_tools.hpp"
#include "framework/render.hpp"
#include "framework/tools.hpp"
#include "framework/window_io.hpp"
#include "game/game_io.hpp"
#include "update_handler.hpp"

using json = nlohmann::json;

wg::GameContext::GameContext(wg::WindowContext& c, wg::ResourceManager& r, wg::UpdateHandler& u)
    : io_(c, r, u, board_), update_handler(u)
{
}

void wg::GameContext::init()
{
    load_config("config.json");
    io_.init();
    update_handler.update(wg::ServUpdate{"join", io_.user_});
    update_handler.update(wg::ConfUpdate{"join", io_.user_});
}

void wg::GameContext::update()
{
    // Poll once per tick for now
    // This is input, not output
    const auto ou = update_handler.poll_game(true);
    if (ou)
    {
        const auto u = *ou;
        set_tile(u.col, u.row, u.c);
        return;
    }
    const auto ocu = update_handler.poll_chat(true);
    if (ocu)
    {
        const auto cu = *ocu;
        // wg::log::data("Received chat update", cu.message, cu.sender);
        io_.chat(cu.message, cu.sender);
        return;
    }
    const auto oconf = update_handler.poll_conf(true);
    if (oconf)
    {
        const auto conf = *oconf;
        wg::log::point("Updating configuration in some way.");
        if (conf.config == "command")
        {
            if (conf.setting == wg::command::host)
            {
                // We are now the host
                io_.chat_broadcast(io_.user_ + " is now host!", "Server");
                // This is a lie, this isn't the server...
                // Oh well.

                // We're the host now.
                is_host_ = true;
                return;
            }
            return;
        }
        if (conf.config == "join")
        {
            players_.insert(conf.setting);
            auto temp_playerlist = players_;
            temp_playerlist.insert(io_.user_);
            if (is_host_) update_handler.update(wg::ConfUpdate{"playerlist", wg::encode_range(players_)});
            io_.chat(conf.setting + " has joined the game!", "Server");
            return;
        }
        if (conf.config == "disconnect")
        {
            players_.erase(conf.setting);
            io_.chat(conf.setting + " has disconnected from the game!", "Server");
            return;
        }
        if (conf.config == "playerlist")
        {
            const auto np = wg::decode_range(conf.setting);
            for (auto&& p : np) if (p != io_.user_) players_.insert(p);
            wg::log::point("Updated playerlist.");
            return;
        }
        if (conf.config == "layout")
        {
            io_.chat("Updating layout.", "Server");
            if (const auto l = wg::try_parse(conf.setting); l)
            {
                board_.set_layout(*l);
            }
            return;
        }
        else
        {
            wg::log::warn(
                "Found configuration update that could not be understood by the game context:",
                "\n\tName: ", conf.config, "\n\tValue: ", conf.setting);
        }
        return;
    }
    const auto ojsonu = update_handler.poll_json(true);
    if (ojsonu)
    {
        const auto jsonu = *ojsonu;
        if (jsonu.json_.find("layout") != jsonu.json_.end())
        {
            wg::log::point("Rewriting the board.");
            board_.parse_board_update(jsonu.json_["layout"]);
        }
        else
        {
            wg::log::warn("Found JSON update that could not be understood by the game context:\n",
                          jsonu.json_.dump(2));
        }
    }
}

void wg::GameContext::parse_input(sf::Event& e)
{
    const bool ret = io_.mode_ == GameIO::Mode::Normal;
    io_.do_event(e);
    if (!ret || io_.mode_ != GameIO::Mode::Normal) return;

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

void wg::GameContext::render(wg::Renderer& renderer)
{
    renderer.render(board_.table_);
    renderer.render(io_);
}

// Event handling
void wg::GameContext::parse_key_released(sf::Event& e)
{
    wg::assert_true(e.type == sf::Event::KeyReleased);
    switch (e.key.code)
    {
        case sf::Keyboard::Key::C: load_config("config.json"); return;
        case sf::Keyboard::Key::D: print_debug(); return;
        default: return;
    }
}

void wg::GameContext::parse_text_entered(sf::Event& e)
{
    if (mode_ == Mode::SetTile)
    {
        const auto c = get_char<std::optional<char>>(e.text.unicode);
        if (c)
        {
            const auto [col, row] = *pending_tile_;
            set_tile(col, row, *c);
            update_handler.update(GameUpdate{int(col), int(row), *c});
        }
        mode_ = Mode::Default;
    }
}

void wg::GameContext::set_tile(int col, int row, char c)
{
    auto& item      = board_.table_.at(col, row);
    item.character_ = c;
}

void wg::GameContext::parse_mouse_released(sf::Event& e)
{
    const auto x              = e.mouseButton.x;
    const auto y              = e.mouseButton.y;
    const auto [tw, th]       = board_.table_.get_dimensions();
    const auto [tx, ty]       = board_.table_.position_;
    const auto [tilew, tileh] = board_.table_.get_tile_dimensions();
    const auto [offx, offy]   = board_.table_.get_tile_offsets();

    if (x > tx && x < tx + tw && y > ty && y < ty + th)
    {
        // Okay, we need to figure out which row we're clicking in:
        const unsigned int row = static_cast<int>((x - tx) / (tilew + offx));
        const unsigned int col = static_cast<int>((y - ty) / (tileh + offy));
        // TODO - This should all be cached 100%, I think
        // But only if this is a noticeable performance hit, unlikely

        wg::assert_true(row < board_.table_.table_size && row >= 0 &&
                        col < board_.table_.table_size && col >= 0);

        pending_tile_ = {row, col};
        mode_         = Mode::SetTile;
    }
}
void wg::GameContext::parse_escape() { running_ = false; }

bool wg::GameContext::load_config(std::string filename)
{
    wg::log::point(__func__, ": Loading configuration data.");
    std::ifstream input_file(filename);
    if (!input_file.good())
    {
        wg::log::warn(__func__, ": Could not open file: ", filename);
        return false;
    }
    json config;
    try
    {
        config = json::parse(input_file);
    }
    catch (const json::parse_error& e)
    {
        wg::log::warn(__func__, ": Could not parse json in file: ", filename);
        return false;
    }

    input_file.close();

    if (config.find("board") != config.end()) board_.parse_config(config.at("board"));

    if (config.find("game") != config.end()) parse_config(config.at("game"));

    return true;
}

bool wg::GameContext::parse_config(const nlohmann::json& config)
{
    wg::log::data("Using configuration", config.dump());

    // So we can set things like tile colour, etc
    for (const auto& k : config.items())
    {
        switch (k.value().type())
        {
            case json::value_t::array:
            {
                json arr = k.value().get<json>();
                if (arr.size() == 0) continue;
                if (arr[0].type() == json::value_t::number_unsigned)
                {
                    wg::log::point(">>> Loading unsigned array for key: ", k.key());
                    set_config(k.key(), k.value().get<std::vector<unsigned int>>());
                }
                else
                {
                    wg::log::point(">>> Loading string array for key: ", k.key());
                    set_config(k.key(), k.value().get<std::vector<std::string>>());
                }
                break;
            }
            case json::value_t::string:
            {
                wg::log::point(">>> Loading string for key: ", k.key());
                set_config(k.key(), k.value().get<std::string>());
                break;
            }
            default:
                wg::log::warn(__func__, ": Tried to load unsupported JSON: ", k.value().dump(1));
                break;
        }
    }

    return true;
}

bool wg::GameContext::set_config(std::string name, std::vector<std::string> value) { return true; }

bool wg::GameContext::set_config(std::string name, std::vector<unsigned int> value)
{
    if (name == "background-colour")
    {
        background_ = sf::Color{(sf::Uint8)value[0], (sf::Uint8)value[1], (sf::Uint8)value[2]};
    }
    return true;
}

bool wg::GameContext::set_config(std::string name, std::string value) { return true; }

void wg::GameContext::print_debug()
{
    std::string debug_str = "Debug information:";
    auto item             = [&debug_str](const std::string& name, const std::string& value) {
        debug_str += "\n\t - " + name + ": " + value;
    };
    auto item_b             = [&debug_str](const std::string& name, bool value) {
        debug_str += "\n\t - " + name + ": " + (value ? "True" : "False");
    };
    // This is awesome! auto lambdas are so great!
    auto item_v = [&debug_str](const std::string& name, const auto& range) {
        debug_str += "\n\t - " + name + ": ";
        for (auto&& val : range)
        {
            debug_str += "\n\t\t - " + val;
        }
    };

    // Players
    item("Username", io_.user_);
    item_b("Is Host", is_host_);
    item_b("Is Running", is_host_);
    item_v("Other Users", players_);

    wg::log::point(debug_str);
}
