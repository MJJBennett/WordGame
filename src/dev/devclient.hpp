#ifndef WG_DEVCLIENT_HPP
#define WG_DEVCLIENT_HPP

#include "update_handler.hpp"

namespace wg::dev
{
class Client : public wg::UpdateHandler
{
    void update(const wg::GameUpdate& u) override;
    void update(const wg::ChatUpdate& u) override;
    void update(const wg::ConfUpdate& u) override;
    std::optional<wg::GameUpdate> poll_game(bool) override;
    std::optional<wg::ChatUpdate> poll_chat(bool) override;
    std::optional<wg::ConfUpdate> poll_conf(bool) override;
};
}  // namespace wg::dev

#endif  // WG_DEVCLIENT_HPP
