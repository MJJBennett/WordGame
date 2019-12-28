#ifndef WG_WEB_CLIENT_HPP
#define WG_WEB_CLIENT_HPP

#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include "update_handler.hpp"

namespace wg
{
class WebSocketClient;
namespace web
{
class Client : public wg::UpdateHandler
{
public:
    void launch(std::string address, std::string port);

    void send(std::string message);
    void shutdown(bool block = false);

    void update(const wg::GameUpdate& u) override;
    void update(const wg::ChatUpdate& u) override;
    void update(const wg::ConfUpdate& u) override;
    std::optional<wg::GameUpdate> poll_game(bool) override;
    std::optional<wg::ChatUpdate> poll_chat(bool) override;
    std::optional<wg::ConfUpdate> poll_conf(bool) override;
    std::optional<wg::JSONUpdate> poll_json(bool) override;

    std::optional<std::string> read_once();
    void cache_once();
    std::queue<std::string> read_all();
    size_t num_waiting();

    std::optional<std::string> cache_;

private:
    std::thread client_thread_;
    std::shared_ptr<WebSocketClient> client_;
    bool launched_{false};
};
}  // namespace web
}  // namespace wg

#endif  // WG_WEB_CLIENT_HPP
