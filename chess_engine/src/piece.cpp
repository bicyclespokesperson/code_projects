#include "piece.h"
#include "board.h"
#include "game.h"
#include "king.h"
#include "player.h"
#include "square.h"

class Square;

Piece::Piece(Player& owner, std::string color, Square const& location) : _color(color), _location(&location), _owner(&owner)
{
}

Piece::~Piece()
{
}

bool Piece::is_white()
{
  return _color == "white";
}

bool Piece::move_to(Player& by_player, Square const& to)
{
  // Checks if the move succeeded
  bool result = true;
  Square const& original = location();
  Piece* captured;
  bool in_check = false;
  bool performed_capture = false;

  if (can_move_to(to))
  {

    // If occupied, capture opponent, while storing it in "captured"
    if (Board::get_board().square_at(to.getX(), to.getY()).occupied())
    {
      // Save the piece in case we need to uncapture it later
      captured = &Board::get_board().square_at(to.getX(), to.getY()).occupied_by();
      by_player.capture(*captured);
      Game::opponent_of(by_player).my_pieces().erase(captured);

      performed_capture = true;
    }

    // Move piece, (original square saved above, in case we need to go back)
    // Delete the piece from its original square
    Board::get_board().square_at(location().getX(), location().getY()).remove_occupier();

    // Place piece on new square
    Board::get_board().square_at(to.getX(), to.getY()).set_occupier(*this);

    // Update the piece's location
    this->set_location(Board::get_board().square_at(to.getX(), to.getY()));

    // check for check
    in_check = _owner->my_king().in_check();

    // if in check, undo everything we just did.
    if (in_check)
    {
      // This move is illegal
      result = false;

      // std::set moved piece location back to old location
      Board::get_board().square_at(original.getX(), original.getY()).set_occupier(*this);
      set_location(Board::get_board().square_at(original.getX(), original.getY()));

      if (performed_capture)
      {
        // place piece back in opponent's pieces, and on board
        _owner->uncapture(*captured);
        Game::opponent_of(by_player).my_pieces().insert(captured);
        Board::get_board().square_at(to.getX(), to.getY()).set_occupier(*captured);
        captured->set_location(Board::get_board().square_at(to.getX(), to.getY()));
      }
      else
      {
        Board::get_board().square_at(to.getX(), to.getY()).remove_occupier();
      }
    }
  }
  else
  {
    result = false;
  }

  return result;
}

std::string Piece::color() const
{
  return _color;
}

void Piece::set_location(Square const& square)
{
  this->_location = &square;
}

bool Piece::is_on_square() const
{
  return _location == NULL;
}

Square const& Piece::location() const
{
  return *_location;
}

Player& Piece::owner()
{
  return *_owner;
}

void Piece::set_owner(Player& new_owner)
{
  _owner = &new_owner;
}
