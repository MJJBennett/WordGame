#ifndef WG_WINDOWIO_HPP
#define WG_WINDOWIO_HPP

#include <string>
#include <vector>

namespace wg
{
class WindowContext;
class ResourceManager;

namespace window_io
{
std::string get_string(wg::WindowContext& target, wg::ResourceManager& manager,
                       const std::string& query, const std::vector<std::string>& options);
}
}  // namespace wg

#endif  // WG_WINDOWIO_HPP
