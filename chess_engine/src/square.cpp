#include "square.h"
#include "my_assert.h"

std::ostream& operator<<(std::ostream& os, Square const& self)
{
  os << self.occupier();

  os << "_";

  if (self.is_occupied())
  {
    switch (self.occupier_color())
    {
    case Color::black:
      os << 'b';
      break;
    case Color::white:
      os << 'w';
      break;
    default:
      MY_ASSERT(false, "Square occupier should be black or white");
      break;
    }
  }
  else
  {
    os << "_";
  }

  os << " ";
  return os;
}

bool Square::is_white() const
{
  MY_ASSERT(is_occupied(), "Square color is irrelevant for an empty square");
  return occupier_color() == Color::white;
}
