#include "game/board.hpp"

#include <nlohmann/json.hpp>
#include "debug/log.hpp"
#include "framework/file_io.hpp"

using json = nlohmann::json;

void wg::Board::parse_board_update(const nlohmann::json& update)
{
    try
    {
        for (const auto& k : update.items())
        {
            const char row_id_char = k.key()[0];
            const int row_id       = row_id_char - 'A';
            wg::log::point(k.value().dump());
            const std::vector<int>& row = k.value().get<std::vector<int>>();
            for (size_t col_id = 0; col_id < row.size(); col_id++)
            {
                // This might be some of the ugliest framework code I've ever made
                // Rough times
                table_.at(col_id, row_id).character_ = decode(row.at(static_cast<unsigned int>(col_id)));
            }
        }
    }
    catch (const json::type_error& e)
    {
        wg::log::err("[Board] JSON parsing error: ", e.what());
        wg::log::err("When trying to update JSON:\n", update.dump(2));
    }
}

int wg::Board::score(const std::vector<Tile>& word) const
{
    bool scored_word{false};
    std::optional<Tile> first;
    int word_score{0};
    int word_multiplier{1};
    int other_scores{0};
    int direction{0};
    for (size_t p = 0; p < word.size(); p++)
    {
        if (!(word[p].board_pos_)) continue;
        if (!first)
            first = word[p];
        else
        {
            if (first->board_pos_->x_ != word[p].board_pos_->x_)
            {
                // x differs, horizontal word
                direction = 1;
            }
            else
                direction = 2;
            break;
        }
    }
    for (const auto& l : word)
    {
        if (!l.board_pos_)
        {
            wg::log::warn("Asked to score letter: ", l.letter_, " that is not on the board!");
            continue;
        }
        const auto x = l.board_pos_->x_;
        const auto y = l.board_pos_->y_;
        const auto m = l.multipliers_;

        word_multiplier *= m.get_word();

        // First, score the whole word!
        if (!scored_word)
        {
            if (direction == 1)
            {
                // Horizontal!
                // Scan left
                int px = x;
                while ((px - 1 >= 0) && is_set(px - 1, y))
                {
                    px--;
                }
                // Okay, we're good. Now accumulate the scores.
                // We don't need to worry about multipliers at all.
                // Simply sum up the letter scores for everything.
                while (is_set(px, y))
                {
                    word_score += scores_.of(*table_.at(px, y).character_);
                    px++;
                }
            }
            else if (direction == 2)
            {
                // Vertical!
                // Scan up
                int py = y;
                while ((py - 1 >= 0) && is_set(x, py - 1))
                {
                    py--;
                }
                // Okay, we're good. Now accumulate the scores.
                // We don't need to worry about multipliers at all.
                // Simply sum up the letter scores for everything.
                while (is_set(x, py))
                {
                    word_score += scores_.of(*table_.at(x, py).character_);
                    py++;
                }
            }
            wg::log::point("Scored word at: ", word_score);
            scored_word = true;
        }

        // Do this after logging
        // This is only the ADDITIONAL word score
        word_score += (m.get_char() - 1) * scores_.of(l.letter_);

        // Now, we try and score offshoots
        if (direction == 1)
        {
            // Horizontal! Scan up!
            int sy = -1;
            if ((y > 1 && is_set(x, y - 1)))  // y - 1 is set
            {
                sy = y - 1;  // this is set
                while (sy > 0)
                {
                    if (is_set(x, sy - 1))
                        sy--;
                    else
                        break;
                }
            }
            else if ((y + 1) < table_.table_size && is_set(x, y + 1))
                sy = y;

            // Done scanning
            if (sy != -1)
            {
                int temps((m.get_char() - 1) * scores_.of(l.letter_));
                // sy is the highest point of our offshoot word
                while (sy < table_.table_size && is_set(x, sy))
                {
                    temps += scores_.of(*table_.at(x, sy).character_);
                    sy++;
                }
                other_scores += temps * m.get_word();
            }
        }
        if (direction == 2)
        {
            // Vertical! Scan left!
            int sx = -1;
            if ((x > 1 && is_set(sx - 1, y)))  // x - 1 is set
            {
                sx = x - 1;  // this is set
                while (sx > 0)
                {
                    if (is_set(sx - 1, y))
                        sx--;
                    else
                        break;
                }
            }
            else if ((x + 1) < table_.table_size && is_set(x + 1, y))
                sx = x;

            // Done scanning
            if (sx != -1)
            {
                int temps((m.get_char() - 1) * scores_.of(l.letter_));
                // sy is the highest point of our offshoot word
                while (sx < table_.table_size && is_set(sx, y))
                {
                    temps += scores_.of(*table_.at(sx, y).character_);
                    sx++;
                }
                other_scores += temps * m.get_word();
            }
        }
    }

    return word_score * word_multiplier + other_scores;
}

void wg::Board::set_layout(const nlohmann::json& layout)
{
    try
    {
        for (const auto& k : layout.items())
        {
            const char row_id_char = k.key()[0];
            const int row_id       = row_id_char - 'A';
            wg::log::point(k.value().dump());
            const std::vector<int>& row = k.value().get<std::vector<int>>();
            for (unsigned int col_id = 0; col_id < static_cast<unsigned int>(row.size()); col_id++)
            {
                const unsigned char i = wg::multiplier::decode(
                    row.at(col_id));  // This number represents the multiplier
                table_.at(col_id, row_id).colour_      = multiplier_colour(i);
                table_.at(col_id, row_id).multipliers_ = i;
            }
        }
    }
    catch (const json::type_error& e)
    {
        wg::log::err("[Board] JSON parsing error: ", e.what());
        wg::log::err("When trying to update JSON:\n", layout.dump(2));
    }
}

sf::Color wg::Board::multiplier_colour(unsigned char multipliers) const
{
    switch (multipliers)
    {
        case wg::multiplier::DoubleChar: return colours_[1];
        case wg::multiplier::DoubleWord: return colours_[2];
        case wg::multiplier::TripleChar: return colours_[3];
        case wg::multiplier::TripleWord: return colours_[4];
        case (wg::multiplier::DoubleWord | wg::multiplier::DoubleChar): return colours_[5];
        default: return colours_[0];
    }
}

bool wg::Board::parse_config(const nlohmann::json& config)
{
    wg::log::data("Using configuration", config.dump());

    // So we can set things like tile colour, etc
    for (const auto& k : config.items())
    {
        switch (k.value().type())
        {
            case json::value_t::array:
            {
                json arr = k.value().get<json>();
                if (arr.size() == 0) continue;
                if (arr[0].type() == json::value_t::number_unsigned)
                {
                    wg::log::point(">>> Loading unsigned array for key: ", k.key());
                    set_config(k.key(), k.value().get<std::vector<unsigned int>>());
                }
                else
                {
                    wg::log::point(">>> Loading string array for key: ", k.key());
                    set_config(k.key(), k.value().get<std::vector<std::string>>());
                }
                break;
            }
            case json::value_t::string:
            {
                wg::log::point(">>> Loading string for key: ", k.key());
                set_config(k.key(), k.value().get<std::string>());
                break;
            }
            default:
                wg::log::warn(__func__, ": Tried to load unsupported JSON: ", k.value().dump(1));
                break;
        }
    }

    return true;
}

bool wg::Board::set_config(std::string name, std::vector<std::string> value) { return true; }

bool wg::Board::set_config(std::string name, std::vector<unsigned int> value)
{
    if (name == "tile-colour")
    {
        table_.set_colour(value[0], value[1], value[2]);
        colours_[0] = wg::colour(value[0], value[1], value[2]);
    }
    else if (name == "doublechar-colour")
    {
        colours_[1] = wg::colour(value[0], value[1], value[2]);
    }
    else if (name == "doubleword-colour")
    {
        colours_[2] = wg::colour(value[0], value[1], value[2]);
    }
    else if (name == "triplechar-colour")
    {
        colours_[3] = wg::colour(value[0], value[1], value[2]);
    }
    else if (name == "tripleword-colour")
    {
        colours_[4] = wg::colour(value[0], value[1], value[2]);
    }
    else if (name == "doublechardoubleword-colour")
    {
        colours_[5] = wg::colour(value[0], value[1], value[2]);
    }
    return true;
}

bool wg::Board::set_config(std::string name, std::string value) { return true; }
