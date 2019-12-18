#ifndef WG_MULTIPLIERS_HPP
#define WG_MULTIPLIERS_HPP

namespace wg
{
namespace multiplier
{
unsigned char DoubleWord = 0b0000'0001;
unsigned char DoubleChar = 0b0000'0001 << 1;
unsigned char TripleWord = 0b0000'0001 << 2;
unsigned char TripleChar = 0b0000'0001 << 3;
unsigned char QuadWord   = 0b0000'0001 << 4;
unsigned char QuadChar   = 0b0000'0001 << 5;
};  // namespace multiplier

struct MultiplierMask
{
    unsigned char multipliers_;

    unsigned int get_word() const
    {
        return (multipliers_ & multiplier::DoubleWord) * 2 +
               (multipliers_ & multiplier::TripleWord) * 3 +
               (multipliers_ & multiplier::QuadWord) * 4;
    }
    unsigned int get_char() const
    {
        return (multipliers_ & multiplier::DoubleChar) * 2 +
               (multipliers_ & multiplier::TripleChar) * 3 +
               (multipliers_ & multiplier::QuadChar) * 4;
    }
};
}  // namespace wg
#endif  // WG_MULTIPLIERS_HPP
