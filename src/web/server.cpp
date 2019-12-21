#include "server.hpp"

#include "session.hpp"
#include <boost/beast.hpp>
#include "debug/log.hpp"

namespace websocket = boost::beast::websocket;
using tcp           = boost::asio::ip::tcp;

wg::Server::Server(std::string address, unsigned short port)
    : tcp_acceptor_(ioc_), address_(std::move(address)), port_(port)
{
    wg::log::point("Initializing webserver.");

    boost::beast::error_code ec;

    signals_.async_wait([&](boost::system::error_code const&, int) { this->ioc_.stop(); });

    wg::log::point("[Server] Launching on: ", address_, ':', port_);

    // Our TCP endpoint for incoming connections
    auto endpoint = tcp::endpoint{boost::asio::ip::make_address(address_), port_};

    // Open a TCP socket for accepting connections
    tcp_acceptor_.open(endpoint.protocol(), ec);
    if (wg::log::opt_err(ec, "Could not open a socket")) return;

    // Bind the socket to the endpoint
    tcp_acceptor_.bind(endpoint, ec);
    if (wg::log::opt_err(ec, "Could not bind a socket")) return;

    // Start listening on the socket
    tcp_acceptor_.listen(asio::socket_base::max_listen_connections, ec);
    if (wg::log::opt_err(ec, "Could not listen on the socket")) return;

    wg::log::point("Successfully configured webserver.");

    // Asynchronously accept on an 'acceptance' strand (so the listening 
    // strand never blocks, presumably)
    tcp_acceptor_.async_accept(asio::make_strand(ioc_),
                               beast::bind_front_handler(&Server::on_accept, this));

    wg::log::point("Launching webserver!");
    ioc_.run(); // Blocking call
}

void wg::Server::on_accept(boost::system::error_code const& ec, tcp::socket socket)
{
    if (wg::log::opt_err(ec, "Could not accept a connection")) return;
    
    wg::log::point("[Server] Launching server session.");
    std::make_shared<wg::Session>(std::move(socket))->launch();

    wg::log::point("[Server] > Continuing to accept connections.");
    tcp_acceptor_.async_accept(asio::make_strand(ioc_),
                               beast::bind_front_handler(&Server::on_accept, this));
}
