#ifndef WG_RESOURCEMANAGER_HPP
#define WG_RESOURCEMANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "resourceidentifier.hpp"
#include "assert.hpp"

namespace wg
{
struct Resource;
struct FontResource;

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    Resource* get(const ResourceIdentifier&);
    std::unique_ptr<Resource> load(const ResourceIdentifier&);

    // Convenience
    FontResource* defaultFont()
    {
        wg::abort_if(default_font_ != nullptr);
        return default_font_;
    }

private:
    std::unordered_map<ResourceIdentifier, std::unique_ptr<Resource>> resources_;
    FontResource* default_font_ = nullptr;
};
}  // namespace wg

#endif  // WG_RESOURCEMANAGER_HPP
