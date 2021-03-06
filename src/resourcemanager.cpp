#include "framework/resourcemanager.hpp"

#include "framework/resource.hpp"
#include "assert.hpp"

wg::ResourceManager::ResourceManager() {}

wg::ResourceManager::~ResourceManager() {}

wg::Resource* wg::ResourceManager::get(const ResourceIdentifier& ri)
{
    if (auto it = resources_.find(ri); it != resources_.end())
    {
        return it->second.get();
    }

    return resources_.try_emplace(ri, load(ri)).first->second.get();
}

std::unique_ptr<wg::Resource> wg::ResourceManager::load(const ResourceIdentifier& ri)
{
    switch (ri.type_)
    {
        case ResourceType::font:
        {
            if (default_font_) return std::make_unique<wg::FontResource>(ri.location_);
            auto f        = std::make_unique<wg::FontResource>(ri.location_);
            default_font_ = f.get();
            return std::move(f);
        }
        case ResourceType::text:
            return std::make_unique<wg::TextResource>(
                static_cast<FontResource*>(get(wg::default_resource::font))->font, ri.location_);
        default:
            WG_ABORT();
    }
}

