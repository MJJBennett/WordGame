#include "client.hpp"

int main()
{
    std::make_shared<wg::WebSocketClient>()->launch();
}
