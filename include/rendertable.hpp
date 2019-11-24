#ifndef WG_RENDERTABLE_HPP
#define WG_RENDERTABLE_HPP

#include "table.hpp"

namespace wg
{
struct RenderOptions;

class RenderTable : public Table
{
public:
    static const RenderOptions& get_default_render_opts();

public:
    bool has_render_opts() const noexcept override { return true; }
    const RenderOptions& get_render_opts() const override;
};
}  // namespace wg

#endif  // WG_RENDERTABLE_HPP
