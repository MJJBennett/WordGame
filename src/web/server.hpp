#ifndef WG_SERVER_HPP
#define WG_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <optional>
#include <memory>
#include "connections.hpp"

namespace wg
{
namespace http  = boost::beast::http;
namespace beast = boost::beast;
namespace asio = boost::asio;

class Server
{
public:
    Server(std::string address, unsigned short port = 27600);

    void on_accept(boost::system::error_code const&, asio::ip::tcp::socket);

private:
    asio::io_context ioc_;
    asio::signal_set signals_{ioc_, SIGINT, SIGTERM};
    asio::ip::tcp::acceptor tcp_acceptor_;
    wg::Connections connections_;

    std::string address_;
    unsigned short port_;
};

}  // namespace wg

#endif  // WG_SERVER_HPP
