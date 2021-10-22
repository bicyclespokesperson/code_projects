#include "player.h"
#include "board.h"
#include "coordinates.h"
#include "square.h"

Player::Player(std::string name) : m_name(std::move(name))
{
}

Player::~Player() = default;

// Returns empty if the player resigns.
std::optional<std::string> Player::prompt_move(std::istream& in, std::ostream& out) const
{
  std::string line = "";
  out << get_name() << ", please enter the beginning and ending squares of the ";
  out << "move (ex: A2 A4): ";

  // Get move from the user and ensure that it is of the correct form
  getline(in, line);
  if (line == "quit")
  {
    // Empty if the player resigned
    return {};
  }
  return line;
}

std::string const& Player::get_name() const
{
  return m_name;
}
