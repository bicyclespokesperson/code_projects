#include <coordinates.h>

std::ostream& operator<<(std::ostream& os, Coordinates const& self)
{
  os << "{" << std::to_string(self.x()) << ", " << std::to_string(self.y()) << "}";
  return os;
}
