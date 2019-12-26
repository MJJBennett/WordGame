#ifndef WG_APPLICATION_HPP
#define WG_APPLICATION_HPP

#include <string>

namespace wg
{
class WindowContext;
class ResourceManager;
class Renderer;

namespace ApplicationMode
{
constexpr unsigned char None = 0b0000'0000;
constexpr unsigned char Windowless = 0b0000'0001;
constexpr unsigned char CustomPort = 0b0000'0001 << 1;
}  // namespace ApplicationMode

class Application
{
public:
    using Mode = unsigned char;

public:
    // Blocking call, starts application
    int launch(Mode mode = ApplicationMode::None);
    void set_address(std::string address);
    void set_port(std::string port);

private:
    int run_webserver(wg::WindowContext& window, wg::ResourceManager& manager,
                      wg::Renderer& renderer);
    int run_webclient(wg::WindowContext& window, wg::ResourceManager& manager,
                      wg::Renderer& renderer);
    int run_local(wg::WindowContext& window, wg::ResourceManager& manager, wg::Renderer& renderer);
    int run_windowless();

    int run_webserver(std::string address);

    std::string address_{};
    std::string port_{};
};
}  // namespace wg

#endif  // WG_APPLICATION_HPP
