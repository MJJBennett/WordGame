#include "application.hpp"

int main(int argc, char* argv[])
{
    auto mode = wg::ApplicationMode::None;
    auto sw   = [](const std::string& str, const std::string& start) {
        return ((start.size() <= str.size()) && str.substr(0, start.size()) == start);
    };
    auto get_arg = [](const std::string& str) -> std::string {
        const auto it = str.find("=");
        if (it == std::string::npos) return "";
        return str.substr(it + 1, str.size());
    };
    wg::Application app;
    for (int i = 0; i < argc; i++)
    {
        if (sw(argv[i], "--s"))
        {
            mode |= wg::ApplicationMode::Windowless;
            if (auto arg = get_arg(argv[i]); arg.size() > 0) app.set_address(arg);
        }
        else if (sw(argv[i], "--p"))
        {
            mode |= wg::ApplicationMode::CustomPort;
            if (auto arg = get_arg(argv[i]); arg.size() > 0) app.set_port(arg);
        }
        else if (sw(argv[i], "--d"))
        {
            mode |= wg::ApplicationMode::Developer;
        }
    }
    return app.launch(mode);
}
