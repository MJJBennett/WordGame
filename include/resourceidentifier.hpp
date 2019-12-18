#ifndef WG_RESOURCEIDENTIFIER_HPP
#define WG_RESOURCEIDENTIFIER_HPP

#include <string>

namespace wg
{
namespace default_resource
{
const ResourceIdentifier font{ResourceType::font, "../resources/Roboto/Roboto-Black.ttf"};
}

enum class ResourceType : int
{
    font,
    text,
};

struct ResourceIdentifier
{
    ResourceType type_;
    std::string location_;
    bool operator==(const ResourceIdentifier& other) const
    {
        return (type_ == other.type_ && location_ == other.location_);
    }
};
}  // namespace wg

namespace std
{
template <>
struct hash<wg::ResourceIdentifier>
{
    std::size_t operator()(const wg::ResourceIdentifier& ri) const
    {
        return std::hash<std::string>()(ri.location_) ^
               (std::hash<int>()(static_cast<int>(ri.type_)) << 1);
    }
};
}  // namespace std

#endif  // WG_RESOURCEIDENTIFIER_HPP
