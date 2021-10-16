#include "king.h"
#include "board.h"
#include "game.h"
#include "player.h"
#include "square.h"

King::King(Player& owner, Color color, Square const& location) : RestrictedPiece(owner, color, location)
{
}

King::~King() = default;

bool King::in_check() const
{
  auto const& opposing_pieces = Game::opponent_of(owner()).my_pieces();
  return std::any_of(opposing_pieces.cbegin(), opposing_pieces.cend(), [this](auto const& piece)
                     {
                       return piece->can_move_to(location());
                     });
}

bool King::can_move_to(Square const& target) const
{
  if (Board::get_board().distance_between(location(), target) != 1)
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

int King::value() const
{
  return 10;
}

void King::display(std::ostream& out) const
{
  char color = (is_white()) ? 'w' : 'b';
  out << "K_" << color << " ";
}
