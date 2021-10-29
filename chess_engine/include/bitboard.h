#ifndef BITBOARD_H
#define BITBOARD_H

#include "coordinates.h"

struct Bitboard
{
  Bitboard(uint64_t val)
  : val(val)
  {
  }

  void set_square(Coordinates coords)
  {
    val |= (uint64_t{0x01} << (coords.y() * 8 + coords.x()));
  }

  void set_square(size_t index)
  {
    val |= (uint64_t{0x01} << index);
  }

  void unset_square(Coordinates coords)
  {
    val &= ~(uint64_t{0x01} << (coords.y() * 8 + coords.x()));
  }

  void unset_square(size_t index)
  {
    val &= ~(uint64_t{0x01} << index);
  }

  bool square_at(size_t index) const
  {
    return static_cast<bool>(val & (uint64_t{0x01} << index));
  }

  Bitboard operator&(Bitboard other) const
  {
    return Bitboard(this->val & other.val);
  }

  Bitboard operator|(Bitboard other) const
  {
    return Bitboard(this->val & other.val);
  }

  Bitboard operator<<(Bitboard other) const
  {
    return Bitboard(this->val & other.val);
  }

  Bitboard operator>>(Bitboard other) const
  {
    return Bitboard(this->val & other.val);
  }

  Bitboard operator~() const
  {
    return ~Bitboard(this->val).val;
  }

  Bitboard operator==(Bitboard other) const
  {
    return val == other.val;
  }

  /*
   * Return the number of one bits in the number
   */
  size_t occupancy() const
  {
    // Kernighan's algorithm
    size_t count = 0;
    uint64_t value{this->val};

    while (value) {
       ++count;
       value &= value - 1; // reset LS1B
    }
    return count;
  }

  /*
   * A bitboard is stored with LSB = a1, up to MSB = h8
   * Order: a1, b1, c1, ..., h1, a2, b2, ... h8
   */
  uint64_t val;
};

std::ostream& operator<<(std::ostream& os, Bitboard const& self);


#endif // BITBOARD_H
