#ifndef WG_SERVER_HPP
#define WG_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <optional>

namespace wg
{
namespace http  = boost::beast::http;
namespace beast = boost::beast;

class Server
{
    class Session : public boost::enable_shared_from_this<Session>
    {
    public:
        Session(boost::asio::ip::tcp::socket&& socket) : tcp_stream_(std::move(socket)) {}

        void launch();
        void start_read();
        void on_read(beast::error_code, std::size_t);
        void on_write(beast::error_code, std::size_t, bool);

    private:
        std::optional<http::request_parser<http::string_body>> req_parser_;
        beast::tcp_stream tcp_stream_;
        beast::flat_buffer buffer_;
    };

public:
    Server();

    void on_accept(boost::system::error_code const&, boost::asio::ip::tcp::socket);

private:
    boost::asio::io_context ioc_;
    boost::asio::signal_set signals_{ioc_, SIGINT, SIGTERM};
    boost::asio::ip::tcp::acceptor tcp_acceptor_;
};

class WebSocketSession : public boost::enable_shared_from_this<WebSocketSession>
{
public:
    WebSocketSession(boost::asio::ip::tcp::socket&& socket) : websocket_(std::move(socket)) {}

    void launch(http::request<http::string_body> req);
    void start_read();
    void on_read(beast::error_code, std::size_t);
    void on_write(beast::error_code, std::size_t, bool);
    void on_accept(beast::error_code);

private:
    beast::websocket::stream<beast::tcp_stream> websocket_;
    beast::flat_buffer buffer_;
};
}  // namespace wg

#endif  // WG_SERVER_HPP
