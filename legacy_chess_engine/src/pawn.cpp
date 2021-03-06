
#include "pawn.h"
#include "board.h"
#include "piece.h"
#include "queen.h"
#include "square.h"

Pawn::Pawn(Player& owner, Color color, Square const& location)
    : RestrictedPiece(owner, color, location)
{
}

Pawn::~Pawn()
{
  if (_proxy)
  {
    delete _proxy;
  }
}

void Pawn::set_proxy(Piece& proxy)
{
  _proxy = &proxy;
}

int Pawn::value() const
{
  return 1;
}

bool Pawn::can_move_to(Square const& target) const
{

  if (_proxy != nullptr)
  {
    return _proxy->can_move_to(target);
  }

  // A pawn can move two spaces on its first move
  int const max_distance = has_moved() ? 1 : 2;
  auto const& board = Board::get_board();

  // Make sure the distance of the move is not greater than 1, or 2 if the
  // piece has not yet moved.
  int const move_distance = board.distance_between(location(), target);
  if (move_distance <= max_distance)
  {
    // Make sure the pawn is moving forward
    if ((is_white() && target.get_y() > location().get_y()) ||
        (!is_white() && target.get_y() < location().get_y()))
    {

      // Ensure the space ahead of the pawn is clear and vertical
      // This also prevents moves of zero spaces, since it cannot move to
      // a space occupied by itself.
      if (board.is_clear_vertical(location(), target) &&
          !(board.square_at(target.get_x(), target.get_y()).occupied()))
      {
        return true;
      }
      // If the square is diagonally forward and occupied by an opponent
      else if (move_distance == 1 &&
               board.is_clear_diagonal(location(), target) &&
               board.square_at(target.get_x(), target.get_y()).occupied() &&
               board.square_at(target.get_x(), target.get_y()).occupied_by().color() != color())
      {
        return true;
      }
    }
  }

  return false;
}

bool Pawn::move_to(Player& by_player, Square const& to)
{
  bool move_succeeded = RestrictedPiece::move_to(by_player, to);

  // Promote pawn if it is on the eighth row
  if (move_succeeded && (to.get_y() == 0 || to.get_y() == 7) && _proxy == nullptr)
  {
    set_proxy(*(new Queen(owner(), ((is_white()) ? Color::white : Color::black), location())));
  }

  if (_proxy != nullptr)
  {
    _proxy->set_location(to);
  }

  return move_succeeded;
}

void Pawn::display(std::ostream& out) const
{
  if (_proxy)
  {
    _proxy->display(out);
  }
  else
  {
    char color = (is_white()) ? 'w' : 'b';
    out << "P_" << color << " ";
  }
}
