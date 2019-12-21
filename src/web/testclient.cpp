#include "client.hpp"

int main()
{
    std::make_shared<wg::WebSocketClient>("127.0.0.1")->launch();
}
