#ifndef WG_GAME_UPDATE_HPP
#define WG_GAME_UPDATE_HPP

#include <nlohmann/json.hpp> // Should be json_fwd
#include <string>

namespace wg
{
struct GameUpdate
{
    int col;
    int row;
    char c;
};

struct ChatUpdate
{
    std::string message;
    std::string sender;
};

struct ConfUpdate
{
    std::string config;
    std::string setting;
};

struct JSONUpdate
{
    const nlohmann::json& json_;
};

struct ServUpdate
{
    std::string update_type;
    std::string update;
};
}  // namespace wg

#endif  // WG_GAME_UPDATE_HPP
