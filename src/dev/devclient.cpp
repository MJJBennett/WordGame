#include "devclient.hpp"

#include <nlohmann/json.hpp>
#include "framework/file_io.hpp"
#include "framework/tools.hpp"

const nlohmann::json default_layout{{"layout",
                                     {"A", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"B", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"C", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"D", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"E", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"F", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"G", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"H", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"I", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"J", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"K", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"L", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"M", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"N", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                     {"O", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}}};
const std::string default_layout_str{default_layout.dump()};

void wg::dev::Client::update(const wg::GameUpdate& u) {}
void wg::dev::Client::update(const wg::ChatUpdate& u)
{
    std::string msg = u.message;
    std::string arg = wg::get_arg(msg);

    if (msg == "open")
    {
        file_ = arg;
        wg::ensure_file_exists(file_, default_layout_str);
    }
}
void wg::dev::Client::update(const wg::ConfUpdate& u) {}
std::optional<wg::GameUpdate> wg::dev::Client::poll_game(bool) { return {}; }
std::optional<wg::ChatUpdate> wg::dev::Client::poll_chat(bool) { return {}; }
std::optional<wg::ConfUpdate> wg::dev::Client::poll_conf(bool) { return {}; }
