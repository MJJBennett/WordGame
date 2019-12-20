#include "client.hpp"

int main()
{
    boost::make_shared<wg::WebSocketClient>()->launch();
}
