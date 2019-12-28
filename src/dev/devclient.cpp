#include "devclient.hpp"

void wg::dev::Client::update(const wg::GameUpdate& u) {}
void wg::dev::Client::update(const wg::ChatUpdate& u) {}
void wg::dev::Client::update(const wg::ConfUpdate& u) {}
std::optional<wg::GameUpdate> wg::dev::Client::poll_game(bool) {}
std::optional<wg::ChatUpdate> wg::dev::Client::poll_chat(bool) {}
std::optional<wg::ConfUpdate> wg::dev::Client::poll_conf(bool) {}
