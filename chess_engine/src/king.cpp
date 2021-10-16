#include "king.h"
#include "board.h"
#include "game.h"
#include "player.h"
#include "square.h"

King::King(Player& owner, std::string color, Square const& location) : RestrictedPiece(owner, color, location)
{
}

King::~King()
{
}

bool King::in_check()
{
  bool in_check = false;

  for (std::set<Piece*>::iterator it = Game::opponent_of(owner()).my_pieces().begin();
       it != Game::opponent_of(owner()).my_pieces().end() && !in_check; it++)
  {
    if ((*it)->can_move_to(owner().my_king().location()))
    {
      (*it)->display(std::cout);
      in_check = true;
    }
  }
  return in_check;
}

bool King::can_move_to(Square const& location) const
{
  bool result = true;

  if (Board::get_board().distance_between(this->location(), location) != 1)
  {
    result = false;
  }

  // If the target location is occupied by a friend, the move is invalid
  if (Board::get_board().square_at(location.get_x(), location.get_y()).occupied() &&
      Board::get_board().square_at(location.get_x(), location.get_y()).occupied_by().color() == color())
  {
    result = false;
  }

  return result;
}

int King::value() const
{
  return 10;
}

void King::display(std::ostream& out) const
{
  char color = (this->color() == "black") ? 'b' : 'w';
  out << "K_" << color << " ";
}
