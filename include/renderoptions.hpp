#ifndef WG_RENDEROPTIONS_HPP
#define WG_RENDEROPTIONS_HPP

#include <optional>
#include <string>
#include <unordered_map>

namespace wg
{
struct RenderOptions
{
    std::unordered_map<std::string, int> opts_;
    std::optional<int> value(std::string s) const
    {
        if (auto it = opts_.find(s); it != opts_.end())
        {
            return it->second;
        }
        return {};
    }
};
}  // namespace wg

#endif  // WG_RENDEROPTIONS_HPP
