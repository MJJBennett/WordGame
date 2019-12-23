#include "application.hpp"

int main(int argc, char* argv[])
{
    auto mode = wg::ApplicationMode::None;
    for (int i = 0; i < argc; i++)
    {
        if (std::string(argv[i]) == "--server") mode |= wg::ApplicationMode::Windowless;
    }
    wg::Application app;
    return app.launch(mode);
}
