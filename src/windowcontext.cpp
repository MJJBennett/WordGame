#include "framework/windowcontext.hpp"

#include "debug/log.hpp"

wg::WindowContext::~WindowContext()
{
    wg::log::point("Destroying window context.");
}
