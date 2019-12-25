#include "game/context.hpp"

#include <SFML/Window/Event.hpp>
#include <nlohmann/json.hpp>
#include "assert.hpp"
#include "debug/log.hpp"
#include "framework/file_io.hpp"
#include "framework/render.hpp"
#include "framework/tools.hpp"
#include "game/game_io.hpp"

using json = nlohmann::json;

wg::GameContext::GameContext(wg::WindowContext& c, wg::ResourceManager& r) : io_(c, r) {}

void wg::GameContext::parse_input(sf::Event& e)
{
    io_.do_event(e);
        
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
    if (e.key.code == sf::Keyboard::Key::C)
    {
        load_config("config.json");
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
            last_update = GameUpdate{int(col), int(row), *c};
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
    wg::log::data("Using configuration", config.dump(2));

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
