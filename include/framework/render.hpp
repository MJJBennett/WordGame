#ifndef WG_RENDER_HPP
#define WG_RENDER_HPP

#include <SFML/Graphics/RectangleShape.hpp>
#include <optional>
#include <string>
#include <unordered_map>

// Forward declaration
namespace sf
{
class RenderWindow;
class Drawable;
}  // namespace sf

namespace wg
{
class Item;
template <typename T>
class Table;
class ResourceManager;

struct RenderOptions
{
    std::unordered_map<std::string, int> opts_;
    std::optional<int> value(std::string s) const
    {
        if (auto it = opts_.find(s); it != opts_.end())
        {
            return it->second;
        }
        return {};
    }
};

class Renderer
{
public:
    enum class Mode
    {
        vertical,
    };

public:
    Renderer(sf::RenderWindow& window, wg::ResourceManager& manager);

    void render(const sf::Drawable&);
    void render(const Table<Item>&);

private:
    sf::RectangleShape rect_;
    sf::RenderWindow& window_;
    wg::ResourceManager& manager_;

    Mode mode_{Mode::vertical};
};

template <typename T>
class Renderable : public T
{
public:
    Renderable(T&& t) : T(std::move(t)) {}
    Renderable(const T& t) : T(t) {}

    wg::RenderOptions opts_;
};

template <typename T>
auto enable_render(T&& obj) -> Renderable<T>
{
    return Renderable<T>{obj};
}
}  // namespace wg

#endif  // WG_RENDER_HPP
