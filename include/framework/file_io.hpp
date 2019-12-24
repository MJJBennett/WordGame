#ifndef WG_FILEIO_HPP
#define WG_FILEIO_HPP

#include <array>
#include <fstream>
#include <string>
#include <vector>

namespace wg
{
enum class ReadMode
{
    All,
    NonEmpty,
    HasChars
};

template <typename T, typename R>
bool in(T t, const R& r)
{
    for (auto&& t2 : r)
        if (t == t2) return true;
    return false;
}

template <typename R>
bool has_chars(R r)
{
    std::array<char, 3> non_chars = {' ', '\r', '\n'};
    for (auto&& c : r)
    {
        if (in(c, non_chars)) continue;
        return true;
    }
    return false;
}

static std::vector<std::string> get_lines(std::string filename, ReadMode r = ReadMode::NonEmpty)
{
    std::ifstream input_file(filename);

    if (input_file.fail()) return {};

    std::vector<std::string> list;
    std::string line;
    while (std::getline(input_file, line))
    {
        switch (r)
        {
            case ReadMode::HasChars:
                if (!has_chars(line)) continue;
            case ReadMode::NonEmpty:
                if (line.size() == 0) continue;
            case ReadMode::All: list.push_back(line);
        }
    }
    return list;
}

static void add_line(std::string filename, std::string line)
{
    std::ofstream output_file(filename, std::ios_base::app | std::ios_base::out);
    output_file << line;
    if (line.size() > 0 && line.back() == '\n') return;
    output_file << '\n';
}
}  // namespace wg

#endif  // WG_FILEIO_HPP
