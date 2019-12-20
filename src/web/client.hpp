#ifndef WG_WEBSOCKET_CLIENT_HPP
#define WG_WEBSOCKET_CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <memory>

namespace wg
{
namespace beast     = boost::beast;
namespace http      = beast::http;
namespace websocket = beast::websocket;
namespace asio      = boost::asio;
using tcp           = boost::asio::ip::tcp;

class WebSocketClient : public std::enable_shared_from_this<WebSocketClient>
{
public:
    WebSocketClient();

    void launch();
    void on_write(beast::error_code, std::size_t);

private:
    // This must be constructed first
    boost::asio::io_context ioc_;

    boost::asio::ip::tcp::resolver resolver_;

    // This uses the SSL context and the IOC
    websocket::stream<beast::tcp_stream> ws_;

    std::string persistent_str_{"Hello from client!"};
};
}  // namespace wg

#endif  // WG_WEBSOCKET_CLIENT_HPP
