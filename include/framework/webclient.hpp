#ifndef WG_WEB_CLIENT_HPP
#define WG_WEB_CLIENT_HPP

#include <memory>
#include <thread>
#include <string>
#include <optional>
#include <queue>

namespace wg
{
class WebSocketClient;
namespace web
{
class Client
{
public:
    void launch(std::string address, std::string port);

    void send(std::string message);
    void shutdown(bool block = false);

    std::optional<std::string> read_once();
    std::queue<std::string> read_all();
    size_t num_waiting();

private:
    std::thread client_thread_;
    std::shared_ptr<WebSocketClient> client_;
    bool launched_{false};
};
}  // namespace web
}  // namespace wg

#endif  // WG_WEB_CLIENT_HPP
