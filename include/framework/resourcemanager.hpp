#ifndef WG_RESOURCEMANAGER_HPP
#define WG_RESOURCEMANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "resourceidentifier.hpp"

namespace wg
{
struct Resource;

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    Resource* get(const ResourceIdentifier&);
    std::unique_ptr<Resource> load(const ResourceIdentifier&);

private:
    std::unordered_map<ResourceIdentifier, std::unique_ptr<Resource>> resources_;
};
}  // namespace wg

#endif  // WG_RESOURCEMANAGER_HPP
