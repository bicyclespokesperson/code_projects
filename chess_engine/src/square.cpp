#include "square.h"
#include "my_assert.h"


bool Square::is_white() const
{
  MY_ASSERT(is_occupied(), "Square color is irrelevant for an empty square");
  return occupier_color() == Color::white;
}
