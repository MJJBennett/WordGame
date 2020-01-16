#include "game/settings.hpp"

#include <unordered_set>
#include "framework/tools.hpp"
#include "strings.hpp"

static wg::Setting default_setting(const std::string& at)
{
    if (at == wg::strings::option::enforce_hand)
    {
        return wg::Setting{true};
    }
    return wg::Setting{""};
}

std::optional<wg::Setting> wg::Settings::parse(const std::string& opt, const std::string& set)
{
    const static std::unordered_set<std::string> string_options = {};
    const static std::unordered_set<std::string> bool_options   = {
        wg::strings::option::enforce_hand};
    const static std::unordered_set<std::string> int_options = {};
    if (bool_options.find(opt) != bool_options.end())
    {
        if (wg::startswith(set, "t") || wg::startswith(set, "T") || wg::startswith(set, "y") ||
            wg::startswith(set, "Y"))
        {
            return wg::Setting{true};
        }
        else if (wg::startswith(set, "f") || wg::startswith(set, "F") ||
                 wg::startswith(set, "n") || wg::startswith(set, "N"))
        {
            return wg::Setting{false};
        }
        return std::nullopt;
    }
    if (int_options.find(opt) != int_options.end())
    {
        return wg::Setting{wg::atoi_default(set, 0)};
    }
    return wg::Setting{set};
}

bool wg::Settings::load_command(std::string cmd)
{
    // First try to split the command
    const auto arg = wg::get_arg(cmd, ".");
    if (arg.size() < 1) return false;

    // Now get the actual option and its setting
    const auto l = wg::split(arg, '=');
    if (l.size() < 2) return false;
    const auto& opt = l[0];
    const auto& set = l[1];

    load_default(opt);
    // Return true IFF the option exists
    bool ret           = settings_.find(opt) != settings_.end();
    const auto setting = parse(opt, set);
    if (setting)
        settings_[opt] = *setting;
    else
        return false;

    return ret;
}

void wg::Settings::load_default(const std::string& opt)
{
    if (settings_.find(opt) == settings_.end()) settings_.insert({opt, default_setting(opt)});
}

bool wg::Settings::get_default(const std::string& opt, bool d)
{
    if (settings_.find(opt) == settings_.end()) return d;
    const auto s = settings_.at(opt);
    try
    {
        return std::get<bool>(s.value_);
    }
    catch (const std::bad_variant_access&)
    {
        return {};
    }
}
