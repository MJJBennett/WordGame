#ifndef WG_DEVCLIENT_HPP
#define WG_DEVCLIENT_HPP

#include "update_handler.hpp"
#include <queue>
#include <nlohmann/json.hpp>

namespace wg::dev
{
class Client : public wg::UpdateHandler
{
public:
    void update(const wg::GameUpdate& u) override;
    void update(const wg::ChatUpdate& u) override;
    void update(const wg::ConfUpdate& u) override;
    void update(const wg::ServUpdate& u) override;
    std::optional<wg::GameUpdate> poll_game(bool) override;
    std::optional<wg::ChatUpdate> poll_chat(bool) override;
    std::optional<wg::ConfUpdate> poll_conf(bool) override;
    std::optional<wg::JSONUpdate> poll_json(bool) override;

private:
    std::string file_;
    nlohmann::json data_;
    bool needs_update_{false};

    std::queue<std::string> chat_;
};
}  // namespace wg::dev

#endif  // WG_DEVCLIENT_HPP
