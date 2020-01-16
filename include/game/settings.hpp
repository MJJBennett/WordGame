#ifndef WG_SETTINGS_HPP
#define WG_SETTINGS_HPP

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

namespace wg
{
struct Setting
{
    std::variant<std::string, bool, int> value_;
};

class Settings
{
public:
    const Setting& get(const std::string& param) const { return settings_.at(param); }
    Setting& get(const std::string& param) { return settings_.at(param); }

    bool load_command(std::string cmd);
    void load_default(const std::string& opt);
    [[nodiscard]] std::optional<wg::Setting> parse(const std::string& opt, const std::string& set);

    [[nodiscard]] bool get_default(const std::string& opt, bool d);

private:
    std::unordered_map<std::string, ::wg::Setting> settings_;
};
}  // namespace wg

#endif  // WG_SETTINGS_HPP
