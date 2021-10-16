#include "player.h"
#include "board.h"
#include "piece.h"
#include "square.h"

class Piece;

Player::Player(std::string name, King& my_king, std::set<Piece*>& my_pieces)
    : _name(name), _pieces(my_pieces), _captured(*(new std::set<Piece*>())), _king(&my_king)
{
}

Player::~Player()
{
  delete &_captured;
}

bool Player::make_move()
{
  bool valid_move = false;
  Piece* occupier = NULL;
  bool still_playing = true;
  std::pair<Square*, Square*>* move = NULL;

  // Keep trying to make a move until a valid one is requested, or until
  // the player resigns.
  while (!valid_move && still_playing)
  {
    move = prompt_move(std::cin, std::cout);

    // prompt_move will return null if the player resigns
    if (move == NULL)
    {
      still_playing = false;
    }
    else if (Board::get_board().square_at(move->first->getX(), move->first->getY()).occupied())
    {
      occupier = &Board::get_board().square_at(move->first->getX(), move->first->getY()).occupied_by();

      if (this->my_pieces().find(occupier) == my_pieces().end())
      {
        std::cout << "Please move one of your own pieces." << std::endl;
      }
      else
      {
        valid_move = occupier->move_to(*this, *(move->second));
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

    // Clean up the move
    if (move != NULL)
    {
      delete move->first;
      delete move->second;
      delete move;
    }
  }

  return still_playing;
}

// Returns a null square if the player resigns.
std::pair<Square*, Square*>* Player::prompt_move(std::istream& in, std::ostream& out)
{
  std::string line = "";
  out << get_name() + ", please enter the beginning and ending squares of the ";
  out << "move (ex: A2 A4): ";
  // char from_col = 0;
  // int to_col = 0;
  std::pair<Square*, Square*>* result = NULL;

  // Get move from the user and ensure that it is of the correct form
  getline(in, line);
  while (!is_valid(line))
  {
    out << "Please make sure the move is of the form \"A1 A2\" and stays"
        << " within the bounds of the 8x8 board, or is \"quit\": ";
    getline(in, line);
  }

  if (line != "quit")
  {
    // convert lower case letters to uppercase
    // Only check for > 'a' because is_valid guarantees that the letter is below 'h'
    if (line[0] >= 'a')
      line[0] = line[0] - 32;

    if (line[3] >= 'a')
      line[3] = line[3] - 32;

    // Create a std::pair of squares to return, representing the beginning and
    // ending squares of the desired move. Subtract 48 and 65 from the lines
    // since they are read as ascii values from the console
    // (so '0' is 48 and 'A' is 65), but we want to store them as integers,
    // so we can do array access. This must be deleted by the caller.
    result = new std::pair<Square*, Square*>(new Square(line[0] - 'A', line[1] - '0' - 1),
                                             new Square(line[3] - 'A', line[4] - '0' - 1));
  }

  // Null if the player resigned
  return result;
}

bool Player::is_valid(const std::string& line)
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

King& Player::my_king()
{
  return *_king;
}
