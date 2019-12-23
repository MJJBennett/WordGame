#include "framework/webclient.hpp"

#include "client.hpp"  // the actual web client that we're wrapping
#include "debug/log.hpp"

void wg::web::Client::launch(std::string address, std::string port)
{
    // Launches a web client!
    // This is actually extremely scary and I'm pretty sure very wrong
    // But hey if it works for now, it works for now!
    launched_      = true;
    client_        = std::make_shared<WebSocketClient>(std::move(address), std::move(port));
    client_thread_ = std::thread(&WebSocketClient::launch, client_);
}

void wg::web::Client::send(std::string message)
{
    if (!launched_)
    {
        wg::log::warn("Tried to send a message over web client before launch:\n", message);
        return;
    }
    assert(client_ != nullptr);
    client_->queue_send(message);
}

std::optional<std::string> wg::web::Client::read_once()
{
    assert(client_ != nullptr);
    return client_->read_once();
}

std::queue<std::string> wg::web::Client::read_all()
{
    assert(client_ != nullptr);
    return client_->read_all();
}

size_t wg::web::Client::num_waiting()
{
    if (!launched_) return 0;
    return client_->num_waiting();
}

void wg::web::Client::shutdown(bool block)
{
    if (!launched_) return;
    assert(client_ != nullptr);
    client_->queue_shutdown();
    if (block) client_thread_.join();
}
