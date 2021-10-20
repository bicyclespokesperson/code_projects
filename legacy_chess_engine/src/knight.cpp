#include "knight.h"
#include "square.h"

Knight::Knight(Player& owner, Color color, Square const& location) : Piece(owner, color, location)
{
}

Knight::~Knight() = default;

bool Knight::can_move_to(Square const& target) const
{
  // Make sure the move is either two vertical and one horizontal
  if (std::abs(location().get_y() - target.get_y()) == 2 && std::abs(location().get_x() - target.get_x()) == 1)
  {
    return true;
  }

  // Or two horizontal and one vertical
  else if (std::abs(location().get_x() - target.get_x()) == 2 && std::abs(location().get_y() - target.get_y()) == 1)
  {
    return true;
  }

  return false;
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
