#include "rook.h"
#include "board.h"
#include "square.h"

class Square;

Rook::Rook(Player& owner, std::string color, Square const& location) : RestrictedPiece(owner, color, location)
{
}

Rook::~Rook()
{
}

bool Rook::can_move_to(Square const& location) const
{
  bool result = true;

  if (!(Board::get_board().is_clear_horizontal(this->location(), location) ||
        Board::get_board().is_clear_vertical(this->location(), location)))
  {
    result = false;
  }

  // If the target location is occupied by a friend, the move is invalid
  if (Board::get_board().square_at(location.getX(), location.getY()).occupied() &&
      Board::get_board().square_at(location.getX(), location.getY()).occupied_by().color() == color())
  {
    result = false;
  }

  return result;
}

int Rook::value() const
{
  return 5;
}

void Rook::display(std::ostream& out) const
{
  char color = (this->color() == "black") ? 'b' : 'w';
  out << "R_" << color << " ";
}
