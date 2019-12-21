#ifndef WG_SERVER_SESSION_HPP
#define WG_SERVER_SESSION_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/string_body.hpp>
#include <memory>
#include <optional>

namespace wg
{
namespace asio  = boost::asio;
namespace beast = boost::beast;
namespace http  = beast::http;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(asio::ip::tcp::socket&& socket);
    ~Session();

    void launch();
    void start_read();
    void on_read(beast::error_code, std::size_t);
    void on_write(beast::error_code, std::size_t, bool);

private:
    std::optional<http::request_parser<http::string_body>> req_parser_;
    beast::tcp_stream tcp_stream_;
    beast::flat_buffer buffer_;
};
}  // namespace wg

#endif  // WG_SERVER_SESSION_HPP
