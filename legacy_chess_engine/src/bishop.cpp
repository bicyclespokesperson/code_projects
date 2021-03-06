#include "bishop.h"
#include "board.h"
#include "square.h"

Bishop::Bishop(Player& owner, Color color, Square const& location) : Piece(owner, color, location)
{
}

Bishop::~Bishop()
{
}

void Bishop::display(std::ostream& out) const
{
  char color = (is_white()) ? 'w' : 'b';
  out << "B_" << color << " ";
}

bool Bishop::can_move_to(Square const& target) const
{
  if (!(Board::get_board().is_clear_diagonal(location(), target)))
  {
    return false;
  }

  // If the target location is occupied by a friend, the move is invalid
  if (auto const& square = Board::get_board().square_at(target.get_x(), target.get_y()); 
      square.occupied() && square.occupied_by().color() == color())
  {
    return false;
  }

  return true;
}

int Bishop::value() const
{
  return 3;
}
