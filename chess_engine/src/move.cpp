#include "move.h"

std::ostream& operator<<(std::ostream& os, Move const& self)
{
  os << self.from << "->" << self.to;
  return os;
}

