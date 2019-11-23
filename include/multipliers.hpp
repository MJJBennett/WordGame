#ifndef WG_MULTIPLIERS_HPP
#define WG_MULTIPLIERS_HPP

namespace wg
{
// This isn't quite as flexible
// as I'd like this to be, but it
// could easily be improved in the future.
// MultiplierMask is just a wrapper anyways.
enum class Multiplier : unsigned char
{
    DoubleWord = 0b0000'0001,
    DoubleChar = 0b0000'0001 << 1,
    TripleWord = 0b0000'0001 << 2,
    TripleChar = 0b0000'0001 << 3,
    QuadWord = 0b0000'0001 << 4,
    QuadChar = 0b0000'0001 << 5,
};

struct MultiplierMask
{
    unsigned char multipliers_;

    unsigned int get_word() const
    {
        return (multipliers_ & 0b0000'0001) * 2 + (multipliers_ & 0b0000'0100) * 3 +
               (multipliers_ & 0b0001'0000) * 4;
    }
    unsigned int get_char() const
    {
        return (multipliers_ & 0b0000'0010) * 2 + (multipliers_ & 0b0000'1000) * 3 +
               (multipliers_ & 0b0010'0000) * 4;
    }
};
}  // namespace wg
#endif  // WG_MULTIPLIERS_HPP
