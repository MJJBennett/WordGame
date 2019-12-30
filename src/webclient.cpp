#include "framework/webclient.hpp"

#include <nlohmann/json.hpp>
#include "assert.hpp"
#include "client.hpp"  // the actual web client that we're wrapping
#include "debug/log.hpp"
#include "framework/tools.hpp"

void wg::web::Client::launch(std::string address, std::string port)
{
    // Launches a web client!
    // This is actually extremely scary and I'm pretty sure very wrong
    // But hey if it works for now, it works for now!
    launched_ = true;

    client_        = std::make_shared<WebSocketClient>(std::move(address), std::move(port));
    client_thread_ = std::thread(&WebSocketClient::launch, client_);
}

void wg::web::Client::update(const wg::GameUpdate& u)
{
    const auto j = nlohmann::json{{"col", u.col}, {"row", u.row}, {"char", u.c}};
    wg::log::point(__func__, ": Sending data: ", j.dump());
    send(j.dump());
}

void wg::web::Client::update(const wg::ChatUpdate& u)
{
    const auto j = nlohmann::json{{"message", u.message}, {"sender", u.sender}};
    wg::log::point(__func__, ": Sending data: ", j.dump());
    send(j.dump());
}

void wg::web::Client::update(const wg::ConfUpdate& u)
{
    const auto j = nlohmann::json{{u.config, u.setting}};
    wg::log::point(__func__, ": Sending data: ", j.dump());
    send(j.dump());
}

void wg::web::Client::update(const wg::ServUpdate& u)
{
    const auto j = nlohmann::json{{"SERVER", u.update_type}, {u.update_type, u.update}};
    wg::log::point(__func__, ": Sending data: ", j.dump());
    send(j.dump());
}

std::optional<wg::GameUpdate> wg::web::Client::poll_game(bool clear)
{
    if (!cache_) return {};
    const auto str = *cache_;
    try
    {
        const auto d = nlohmann::json::parse(str);
        if (d.find("col") == d.end()) return {};
        const auto col = d["col"];
        const auto row = d["row"];
        const auto c   = char(int(d["char"]));
        if (clear) cache_.reset();
        return wg::GameUpdate{col, row, c};
    }
    catch (nlohmann::json::parse_error e)
    {
        wg::log::warn(__func__, ": Got json parse error: ", e.what());
        wg::log::warn("\t\tWhile parsing string: ", str);
        return {};
    }
}

std::optional<wg::ChatUpdate> wg::web::Client::poll_chat(bool clear)
{
    if (!cache_) return {};
    const auto str = *cache_;
    try
    {
        const auto d = nlohmann::json::parse(str);
        if (d.find("sender") == d.end()) return {};
        const auto message = d["message"];
        const auto sender  = d["sender"];
        if (clear) cache_.reset();
        return wg::ChatUpdate{message, sender};
    }
    catch (nlohmann::json::parse_error e)
    {
        wg::log::warn(__func__, ": Got json parse error: ", e.what());
        wg::log::warn("\t\tWhile parsing string: ", str);
        return {};
    }
}

std::optional<wg::ConfUpdate> wg::web::Client::poll_conf(bool clear)
{
    if (!cache_) return {};
    const auto str = *cache_;
    if (clear) cache_.reset();
    try
    {
        const auto d = nlohmann::json::parse(str);
        if (d.find("command") != d.end())
        {
            const std::string command = d["command"];
            return wg::ConfUpdate{"command", command};
        }
        if (d.find("layout") != d.end())
        {
            const std::string layout = d["layout"];
            return wg::ConfUpdate{"layout", layout};
        }
        if (d.find("join") != d.end())
        {
            const std::string join = d["join"];
            wg::log::point("New player joined: ", join);
            return wg::ConfUpdate{"join", join};
        }
        if (d.find("disconnect") != d.end())
        {
            const std::string disconnect = d["disconnect"];
            wg::log::point("Player disconnected: ", disconnect);
            return wg::ConfUpdate{"disconnect", disconnect};
        }
        if (d.find("playerlist") != d.end())
        {
            const std::string playerlist = d["playerlist"];
            wg::log::point("New playerlist: ", playerlist);
            return wg::ConfUpdate{"playerlist", playerlist};
        }
        if (d.find("turn") != d.end())
        {
            const std::string turn = d["turn"];
            wg::log::point("New turn: ", turn);
            return wg::ConfUpdate{"turn", turn};
        }
        if (d.find("charset") != d.end())
        {
            const std::string charset = d["charset"];
            wg::log::point("New charset: ", charset);
            return wg::ConfUpdate{"charset", charset};
        }
        return {};
    }
    catch (nlohmann::json::parse_error e)
    {
        wg::log::warn(__func__, ": Got json parse error: ", e.what());
        wg::log::warn("\t\tWhile parsing string: ", str);
        return {};
    }
}

std::optional<wg::JSONUpdate> wg::web::Client::poll_json(bool) { return {}; }

void wg::web::Client::send(std::string message)
{
    if (!launched_)
    {
        wg::log::warn("Tried to send a message over web client before launch:\n", message);
        return;
    }
    wg::assert_true(client_ != nullptr);
    client_->queue_send(message);
}

std::optional<std::string> wg::web::Client::read_once()
{
    wg::assert_true(client_ != nullptr);
    return client_->read_once();
}

void wg::web::Client::cache_once()
{
    auto str = read_once();
    if (has_chars(*str)) cache_ = *str;
}

std::queue<std::string> wg::web::Client::read_all()
{
    wg::assert_true(client_ != nullptr);
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
    wg::assert_true(client_ != nullptr);
    client_->queue_shutdown();
    wg::log::point("Queued shutdown. Blocking.");
    if (block) client_thread_.join();
}
