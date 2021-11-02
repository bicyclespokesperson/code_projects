#include "bitboard.h"

std::ostream& operator<<(std::ostream& os, Bitboard const& self)
{
  for (int y{7}; y >= 0; --y)
  {
    for (int x{0}; x < 8; ++x)
    {
      auto square_index = static_cast<size_t>(y * 8 + x);
      char result = self.square_at(square_index) ? '1' : '.';
      os << result << ' ';
    }
    os << "\n";
  }

  return os;
}
