#ifndef WG_WINDOWIO_HPP
#define WG_WINDOWIO_HPP

#include <string>
#include <tuple>
#include <vector>

namespace wg
{
class WindowContext;
class ResourceManager;

namespace window_io
{
template <typename num>
auto center(num context_length, num item_length) -> num
{
    const auto middle = context_length / 2;
    const auto half   = item_length / 2;
    return middle - half;
}

std::string get_string(wg::WindowContext& target, wg::ResourceManager& manager,
                       const std::string& query, const std::vector<std::string>& options);
void back_screen(wg::WindowContext& target, wg::ResourceManager& manager,
                 const std::string& message, const std::string& button_message = "Back");
}  // namespace window_io
}  // namespace wg

#endif  // WG_WINDOWIO_HPP
