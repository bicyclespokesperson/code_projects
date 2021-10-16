#include "knight.h"
#include "square.h"

Knight::Knight(Player& owner, Color color, Square const& location) : Piece(owner, color, location)
{
}

Knight::~Knight()
{
}

bool Knight::can_move_to(Square const& location) const
{
  bool result = false;

  // Make sure the move is either two vertical and one horizontal
  if (abs(this->location().get_y() - location.get_y()) == 2 && abs(this->location().get_x() - location.get_x()) == 1)
  {
    result = true;
  }

  // Or two horizontal and one vertical
  else if (abs(this->location().get_x() - location.get_x()) == 2 && abs(this->location().get_y() - location.get_y()) == 1)
  {
    result = true;
  }

  return result;
}

int Knight::value() const
{
  return 3;
}

void Knight::display(std::ostream& out) const
{
  char color = (is_white()) ? 'w' : 'b';
  out << "N_" << color << " ";
}
