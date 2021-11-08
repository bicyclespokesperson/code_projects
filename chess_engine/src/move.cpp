#include "move.h"

std::ostream& operator<<(std::ostream& os, Move const& self)
{
  os << self.from << self.to;
  if (self.promotion != Piece::empty)
  {
    std::stringstream ss;
    ss << self.promotion;
    os << static_cast<char>(tolower(ss.str().front()));
  }
  //os << self.from << "->" << self.to;
  return os;
}
