#ifndef WG_APPLICATION_HPP
#define WG_APPLICATION_HPP

namespace wg
{
class WindowContext;
class ResourceManager;
class Renderer;

class Application
{
public:
    // Blocking call, starts application
    int launch();

private:
    int run_webserver(wg::WindowContext& window, wg::ResourceManager& manager, wg::Renderer& renderer);
    int run_webclient(wg::WindowContext& window, wg::ResourceManager& manager, wg::Renderer& renderer);
    int run_local(wg::WindowContext& window, wg::ResourceManager& manager, wg::Renderer& renderer);
};
}  // namespace wg

#endif  // WG_APPLICATION_HPP
