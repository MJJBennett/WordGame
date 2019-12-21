#ifndef WG_WEB_CLIENT_HPP
#define WG_WEB_CLIENT_HPP

#include <memory>
#include <thread>
#include <string>

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

private:
    std::thread client_thread_;
    std::shared_ptr<WebSocketClient> client_;
};
}  // namespace web
}  // namespace wg

#endif  // WG_WEB_CLIENT_HPP
