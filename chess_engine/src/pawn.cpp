
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

bool Pawn::can_move_to(Square const& location) const
{
  bool result = false;
  int max_distance = 1;

  if (_proxy != nullptr)
  {
    result = _proxy->can_move_to(location);
  }
  else
  {
    // A pawn can move two spaces on its first move
    if (!has_moved())
    {
      max_distance = 2;
    }

    // Make sure the distance of the move is not greater than 1, or 2 if the
    // piece has not yet moved.
    if (Board::get_board().distance_between(this->location(), location) <= max_distance)
    {
      // Make sure the pawn is moving forward
      // "this" is needed to distinguish between the parameter location and
      // the method location()
      if ((is_white() && location.get_y() > this->location().get_y()) ||
          (!is_white() && location.get_y() < this->location().get_y()))
      {

        // If the space ahead of the pawn is clear and vertical
        // This also prevents moves of zero spaces, since it cannot move to
        // a space occupied by itself.
        if (Board::get_board().is_clear_vertical(this->location(), location) &&
            !(Board::get_board().square_at(location.get_x(), location.get_y()).occupied()))
        {
          result = true;
        }
        // If the square is diagonally forward and occupied by an opponent
        else if (Board::get_board().is_clear_diagonal(this->location(), location) &&
                 Board::get_board().square_at(location.get_x(), location.get_y()).occupied() &&
                 Board::get_board().square_at(location.get_x(), location.get_y()).occupied_by().color() != this->color())
        {
          result = true;
        }
      }
    }
  }

  return result;
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
  if (_proxy != nullptr)
  {
    _proxy->display(out);
  }
  else
  {
    char color = (is_white()) ? 'w' : 'b';
    out << "P_" << color << " ";
  }
}
