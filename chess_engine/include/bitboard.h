#ifndef BITBOARD_H
#define BITBOARD_H

#include "coordinates.h"

struct Bitboard
{
  constexpr Bitboard() = default;

  constexpr Bitboard(uint64_t value)
  : val(value)
  {
  }

  constexpr void set_square(Coordinates coords)
  {
    val |= (uint64_t{0x01} << (coords.y() * 8 + coords.x()));
  }

  constexpr void set_square(size_t index)
  {
    val |= (uint64_t{0x01} << index);
  }

  constexpr void unset_square(Coordinates coords)
  {
    val &= ~(uint64_t{0x01} << (coords.y() * 8 + coords.x()));
  }

  constexpr void unset_square(size_t index)
  {
    val &= ~(uint64_t{0x01} << index);
  }

  constexpr bool square_at(size_t index) const
  {
    return static_cast<bool>(val & (uint64_t{0x01} << index));
  }

  constexpr Bitboard operator&(Bitboard other) const
  {
    return Bitboard(this->val & other.val);
  }

  constexpr Bitboard operator|(Bitboard other) const
  {
    return Bitboard(this->val | other.val);
  }

  constexpr Bitboard operator<<(Bitboard other) const
  {
    return Bitboard(this->val << other.val);
  }

  constexpr Bitboard operator>>(Bitboard other) const
  {
    return Bitboard(this->val >> other.val);
  }

  constexpr Bitboard operator~() const
  {
    return Bitboard(~this->val);
  }

  constexpr Bitboard operator^(Bitboard other) const
  {
    return Bitboard(this->val ^ other.val);
  }

  constexpr Bitboard operator==(Bitboard other) const
  {
    return val == other.val;
  }

  /*
   * Return the number of one bits in the number
   */
  constexpr int32_t occupancy() const
  {
    // Kernighan's algorithm
    int32_t count = 0;
    uint64_t value{this->val};

    while (value) {
       ++count;
       value &= value - 1; // reset LS1B
    }
    return count;
  }

  constexpr bool is_empty() const
  {
    return val == 0;
  }

  constexpr int32_t bitscan_forward() const {
    if (val == 0) {
      return -1;
    }
    return __builtin_ffsll(val) - 1;
  }

  constexpr int bitscan_reverse() {
    if (val == 0) {
      return -1;
    }
    return 63 - __builtin_clzll(val);
  }

  /*
   * A bitboard is stored with LSB = a1, up to MSB = h8
   * Order: a1, b1, c1, ..., h1, a2, b2, ... h8
   */
  uint64_t val{0};
};

std::ostream& operator<<(std::ostream& os, Bitboard const& self);

#endif // BITBOARD_H
