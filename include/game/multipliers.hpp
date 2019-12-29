#ifndef WG_MULTIPLIERS_HPP
#define WG_MULTIPLIERS_HPP

namespace wg
{
namespace multiplier
{
constexpr unsigned char None       = 0b00;
constexpr unsigned char DoubleWord = 0b01;
constexpr unsigned char DoubleChar = 0b01 << 1;
constexpr unsigned char TripleWord = 0b01 << 2;
constexpr unsigned char TripleChar = 0b01 << 3;
constexpr unsigned char QuadWord   = 0b01 << 4;
constexpr unsigned char QuadChar   = 0b01 << 5;

inline unsigned char decode(int i)
{
    // We're going to go with just a small subset of sane possibilities for now
    switch (i)
    {
        case 0: return ::wg::multiplier::None;
        case 1: return ::wg::multiplier::DoubleChar;
        case 2: return ::wg::multiplier::DoubleWord;
        case 3: return ::wg::multiplier::TripleChar;
        case 4: return ::wg::multiplier::TripleWord;
        case 5: return ::wg::multiplier::QuadWord;
        case 6: return ::wg::multiplier::QuadChar;
        case 7: return ::wg::multiplier::DoubleChar & ::wg::multiplier::DoubleWord;
        case 8: return ::wg::multiplier::TripleChar & ::wg::multiplier::DoubleWord;
        case 9: return ::wg::multiplier::DoubleChar & ::wg::multiplier::TripleWord;
        default: return ::wg::multiplier::None;
    }
}
};  // namespace multiplier

struct MultiplierMask
{
    unsigned char multipliers_;
    MultiplierMask(unsigned char multipliers) : multipliers_(multipliers) {}

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
