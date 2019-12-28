#ifndef WG_UPDATE_HANDLER_HPP
#define WG_UPDATE_HANDLER_HPP

#include <optional>
#include "game/update.hpp"

namespace wg
{
class UpdateHandler
{
public:
    virtual void update(const wg::GameUpdate& u)          = 0;
    virtual void update(const wg::ChatUpdate& u)          = 0;
    virtual void update(const wg::ConfUpdate& u)          = 0;
    virtual std::optional<wg::GameUpdate> poll_game(bool) = 0;
    virtual std::optional<wg::ChatUpdate> poll_chat(bool) = 0;
    virtual std::optional<wg::ConfUpdate> poll_conf(bool) = 0;
};
}  // namespace wg

#endif  // WG_UPDATE_HANDLER_HPP
