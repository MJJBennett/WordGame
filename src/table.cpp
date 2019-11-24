#include "table.hpp"

bool wg::Table::has_render_opts() const noexcept { return false; }

const wg::RenderOptions& wg::Table::get_render_opts() const { assert(false); }
