#include "queen.h"
#include "board.h"
#include "piece.h"
#include "square.h"

Queen::Queen(Player& owner, Color color, Square const& location) : Piece(owner, color, location)
{
}

Queen::~Queen()
{
}

bool Queen::can_move_to(Square const& target) const
{
  bool result = true;

  // If the square is not horizontally, vertically, or diagonally away from the
  // Queen's current location, the move is invalid.
  if (!(Board::get_board().is_clear_diagonal(location(), target) ||
        Board::get_board().is_clear_horizontal(location(), target) ||
        Board::get_board().is_clear_vertical(location(), target)))
  {
    result = false;
  }

  // If the target location is occupied by a friend, the move is invalid
  if (Board::get_board().square_at(target.get_x(), target.get_y()).occupied() &&
      Board::get_board().square_at(target.get_x(), target.get_y()).occupied_by().color() == color())
  {
    result = false;
  }

  return result;
}

int Queen::value() const
{
  return 9;
}

void Queen::display(std::ostream& out) const
{
  char color = (is_white()) ? 'w' : 'b';
  out << "Q_" << color << " ";
}
