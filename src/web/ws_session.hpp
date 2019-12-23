#ifndef WG_WEBSOCKET_SERVER_SESSION
#define WG_WEBSOCKET_SERVER_SESSION

#include <boost/asio/ip/tcp.hpp>             // asio::ip::tcp::socket
#include <boost/beast/core/flat_buffer.hpp>  // beast::flat_buffer
#include <boost/beast/core/tcp_stream.hpp>   // beast::tcp_stream
#include <boost/beast/http/message.hpp>      // http::request
#include <boost/beast/http/string_body.hpp>  // http::string_body
#include <boost/beast/websocket/stream.hpp>  // websocket::stream
#include <memory>                            // std::enable_shared_from_this
#include <queue>
#include <string>

namespace wg
{
namespace http  = boost::beast::http;
namespace beast = boost::beast;
namespace asio  = boost::asio;

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
public:
    WebSocketSession(asio::ip::tcp::socket&& socket);
    ~WebSocketSession();

    void launch(http::request<http::string_body> req);
    void start_read();
    void on_read(beast::error_code, std::size_t);
    void on_write(beast::error_code, std::size_t);
    void on_accept(beast::error_code);

private:
    beast::websocket::stream<beast::tcp_stream> websocket_;
    beast::flat_buffer buffer_;

    std::queue<std::string> write_queue_;
};
}  // namespace wg

#endif  // WG_WEBSOCKET_SERVER_SESSION
