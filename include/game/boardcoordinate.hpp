#ifndef WG_BOARDCOORD_HPP
#define WG_BOARDCOORD_HPP

namespace wg
{
struct BoardCoordinate
{
    BoardCoordinate(int x, int y) : x_(x), y_(y) {}
    int x_;
    int y_;
};
}  // namespace wg

#endif  // WG_BOARDCOORD_HPP
