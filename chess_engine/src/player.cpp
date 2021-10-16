#include "player.h"
#include "board.h"
#include "piece.h"
#include "square.h"

class Piece;

Player::Player(std::string name, King& my_king, std::set<Piece*> my_pieces)
    : _name(std::move(name)), _pieces(std::move(my_pieces)), _king(&my_king)
{
}

Player::~Player()
{
  delete &_captured;
}

bool Player::make_move()
{
  bool valid_move = false;
  bool still_playing = true;

  // Keep trying to make a move until a valid one is requested, or until
  // the player resigns.
  while (!valid_move && still_playing)
  {
    auto move = prompt_move_(std::cin, std::cout);

    if (!move)
    {
      still_playing = false;
    }
    else if (Board::get_board().square_at(move->first.get_x(), move->first.get_y()).occupied())
    {
      Piece* occupier = &Board::get_board().square_at(move->first.get_x(), move->first.get_y()).occupied_by();

      if (this->my_pieces().find(occupier) == my_pieces().end())
      {
        std::cout << "Please move one of your own pieces." << std::endl;
      }
      else
      {
        valid_move = occupier->move_to(*this, (move->second));
        if (!valid_move)
        {
          std::cout << "please enter a valid move for the piece, and ";
          std::cout << "ensure that it does not leave the King in check." << std::endl;
        }
      }
    }
    else
    {
      std::cout << "please choose a square with a piece on it." << std::endl;
    }
  }

  return still_playing;
}

// Returns a null square if the player resigns.
std::optional<std::pair<Square, Square>> Player::prompt_move_(std::istream& in, std::ostream& out)
{
  std::string line = "";
  out << get_name() + ", please enter the beginning and ending squares of the ";
  out << "move (ex: A2 A4): ";

  // Get move from the user and ensure that it is of the correct form
  getline(in, line);
  while (!is_valid_(line) && in.good())
  {
    out << "Please make sure the move is of the form \"A1 A2\" and stays"
        << " within the bounds of the 8x8 board, or is \"quit\": ";
    getline(in, line);
  }

  if (line != "quit" && in.good())
  {
    // convert lower case letters to uppercase
    // Only check for > 'a' because is_valid guarantees that the letter is below 'h'
    if (line[0] >= 'a')
    {
      line[0] = line[0] - 32;
    }

    if (line[3] >= 'a')
    {
      line[3] = line[3] - 32;
    }

    // Create a std::pair of squares to return, representing the beginning and
    // ending squares of the desired move. Subtract 48 and 65 from the lines
    // since they are read as ascii values from the console
    // (so '0' is 48 and 'A' is 65), but we want to store them as integers,
    // so we can do array access.
    return std::pair<Square, Square>{{line[0] - 'A', line[1] - '0' - 1}, {line[3] - 'A', line[4] - '0' - 1}};
  }

  // Null if the player resigned
  return {};
}

bool Player::is_valid_(const std::string& line)
{
  bool result = true;

  // If any character does not  fall in the allowed range,
  // the std::string is invalid. (valid: "A2 A3")
  // Also, "quit" is a valid input.

  // Make sure the std::string is the correct length. This also uses short
  // circuiting to protect the std::string from bounds errors.
  if (line != "quit" && (line.length() != 5 ||

                         // 'A' <= line[0] <= 'H'
                         !((line[0] >= 'A' && line[0] <= 'H') || (line[0] >= 'a' && line[0] <= 'h')) ||

                         // '1' <= line[1] <= '8'
                         line[1] < '1' || line[1] > '8' ||

                         // line[2] should be a space
                         line[2] != ' ' ||

                         // 'A' <= line[3] <= 'H'
                         !((line[3] >= 'A' && line[3] <= 'H') || (line[3] >= 'a' && line[3] <= 'h')) ||

                         // '1' <= line[4] <= '8'
                         line[4] < '1' || line[4] > '8'))
  {
    result = false;
  }

  return result;
}

std::string& Player::get_name()
{
  return _name;
}

int Player::score()
{
  return 0;
}

void Player::capture(Piece& a_piece)
{
  _captured.insert(&a_piece);
}

void Player::uncapture(Piece& a_piece)
{
  _captured.erase(&a_piece);
}

std::set<Piece*>& Player::my_pieces()
{
  return this->_pieces;
}

King& Player::my_king() const
{
  return *_king;
}
