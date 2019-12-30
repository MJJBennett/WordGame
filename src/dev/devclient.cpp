#include "devclient.hpp"

#include <nlohmann/json.hpp>
#include "debug/log.hpp"
#include "framework/file_io.hpp"
#include "framework/tools.hpp"
#include "game/board.hpp"

const nlohmann::json default_layout{{"layout",
                                     {{"A", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
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
                                      {"O", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}}}};
const std::string default_layout_str{default_layout.dump()};

void wg::dev::Client::update(const wg::GameUpdate& u)
{
    const char row_id = u.row + 'A';
    try
    {
        data_["layout"][std::string(1, row_id)][u.col] = wg::Board::encode(u.c);
    }
    catch (const nlohmann::json::type_error& e)
    {
        wg::log::err("JSON parsing error: ", e.what());
        wg::log::err("When trying to update JSON:\n", data_.dump(2));
    }
}

void wg::dev::Client::update(const wg::ChatUpdate& u)
{
    std::string msg = u.message;
    std::string arg = wg::get_arg(msg);
    wg::log::point("[Developer Client] Parsing chat update.");

    if (wg::startswith(msg, std::string{"open"}))
    {
        file_ = arg;
        wg::ensure_file_exists(file_, default_layout_str);

        std::ifstream input_file(file_);
        if (!input_file.good())
        {
            wg::log::warn(__func__, ": Could not open file: ", file_);
            chat_.push("Could not open file: " + file_);
            return;
        }
        try
        {
            data_ = nlohmann::json::parse(input_file);
        }
        catch (const nlohmann::json::parse_error& e)
        {
            wg::log::warn(__func__, ": Could not parse json in file: ", file_);
            chat_.push("Could not parse json in file: " + file_);
            return;
        }
        input_file.close();

        chat_.push("Now editing file: " + file_);
        // We need to pass this JSON to the board so it can get caught up
        needs_update_ = true;
        return;
    }
    if (wg::startswith(msg, std::string{"save"}))
    {
        if (file_.length() == 0) return;
        std::ofstream output_file(file_);
        output_file << std::setw(2) << data_ << std::endl;
        chat_.push("Saved to file: " + file_);
    }
}

void wg::dev::Client::update(const wg::ConfUpdate& u) {}
void wg::dev::Client::update(const wg::ServUpdate& u) {}

std::optional<wg::JSONUpdate> wg::dev::Client::poll_json(bool)
{
    if (needs_update_)
    {
        needs_update_ = false;
        return wg::JSONUpdate{data_};
    }
    else
        return {};
}
std::optional<wg::GameUpdate> wg::dev::Client::poll_game(bool) { return {}; }
std::optional<wg::ChatUpdate> wg::dev::Client::poll_chat(bool)
{
    if (chat_.size() != 0)
    {
        const auto chat = chat_.front();
        chat_.pop();
        return wg::ChatUpdate{chat, "Editor"};
    }
    return {};
}
std::optional<wg::ConfUpdate> wg::dev::Client::poll_conf(bool) { return {}; }
