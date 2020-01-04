#ifndef WG_SCORING_HPP
#define WG_SCORING_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace wg
{
class ScoreMap
{
public:
    void from_file(const std::string&);
    void from_string(const std::string&);
    void from_json(const nlohmann::json&);
    int of(char c) const { return map_.at(c); }

    const std::unordered_map<char, int>& get_ref() const { return map_; }

private:
    std::unordered_map<char, int> map_;
};
}  // namespace wg

#endif  // WG_SCORING_HPP
