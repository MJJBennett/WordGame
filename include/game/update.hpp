#ifndef WG_GAME_UPDATE_HPP
#define WG_GAME_UPDATE_HPP

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
}  // namespace wg

#endif  // WG_GAME_UPDATE_HPP
