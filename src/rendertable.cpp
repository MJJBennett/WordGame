#include "rendertable.hpp"

#include "renderoptions.hpp"

const static wg::RenderOptions default_render_opts{};

const wg::RenderOptions& wg::RenderTable::get_default_render_opts() { return default_render_opts; }
