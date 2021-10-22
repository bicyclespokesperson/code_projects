#include "player.h"
#include "board.h"
#include "coordinates.h"
#include "square.h"

Player::Player(std::string name) : m_name(std::move(name))
{
}

Player::~Player() = default;

// Returns empty if the player resigns.
std::optional<std::pair<Coordinates, Coordinates>> Player::prompt_move(std::istream& in, std::ostream& out) const
{
  std::string line = "";
  out << get_name() << ", please enter the beginning and ending squares of the ";
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
    return std::pair<Coordinates, Coordinates>{
        {static_cast<int8_t>(line[0] - 'A'), static_cast<int8_t>(line[1] - '1')},
        {static_cast<int8_t>(line[3] - 'A'), static_cast<int8_t>(line[4] - '1')}};
  }

  // Empty if the player resigned
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

std::string const& Player::get_name() const
{
  return m_name;
}
