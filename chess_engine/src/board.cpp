#include "board.h"

constexpr int c_board_dimension{8};

Board& Board::get_board()
{
  if (_theBoard == nullptr)
  {
    // Use new to access private constructor
    _theBoard = std::unique_ptr<Board>(new Board());
  }
  return *_theBoard;
}

Board::Board()
{
  for (int i = 0; i < c_board_dimension; i++)
  {
    for (int j = 0; j < c_board_dimension; j++)
    {
      // The matrix is compressed into a one dimensional array and stored
      // in row major form.
      _squares.emplace_back(i, j, nullptr);
    }
  }
}

Board::~Board() = default;

/**
 * Calculates the distance between two squares on the board
 * @param from The first square
 * @param to The second square
 * @return The resulting distance
 */
int Board::distance_between(Square const& from, Square const& to)
{
  int result = -1;

  // If the squares are on the same vertical, return the difference
  // in their horizontals
  if (from.get_x() == to.get_x())
  {
    result = std::abs(from.get_y() - to.get_y());
  }

  // If the squares are on the same horizontal, return the difference
  // in their verticals
  else if (from.get_y() == to.get_y())
  {
    result = std::abs(from.get_x() - to.get_x());
  }

  // If the squares are on the same diagonal, their x and y differences
  // will be equal, so return their X difference
  else if (std::abs(from.get_x() - to.get_x()) == std::abs(from.get_y() - to.get_y()))
  {
    result = std::abs(from.get_x() - to.get_x());
  }

  return result;
}

void Board::setup()
{
  _squares.clear();
  for (char i = 0; i < c_board_dimension; i++)
  {
    for (int j = 0; j < c_board_dimension; j++)
    {
      // The squares should be stored with their zero based xy coordinates
      // so that the array access math works out nicely.
      _squares.emplace_back(i, j, nullptr);
    }
  }
}

Square& Board::square_at(int x, int y)
{
  return _squares.at(c_board_dimension * x + y);
}

bool Board::is_clear_vertical(Square const& from, Square const& to)
{
  bool result = true;
  Square const* top = &to;
  Square const* bottom = &from;
  // Set up the counter for the while loop.
  // Assume we are going to start from one space ahead of the current square
  // and walk straight until we hit the destination square

  if (from.get_x() != to.get_x())
  {
    result = false;
  }
  else
  {
    // Set up the squares so we always walk up.
    // So if to is above from, swap them.
    if (to.get_y() < from.get_y())
    {
      bottom = &to;
      top = &from;
    }

    // Walk along the board and if we find an occupied space, exit the loop
    // and return false.
    for (int i = bottom->get_y() + 1; i < top->get_y() && result; i++)
    {
      if (square_at(from.get_x(), i).occupied())
      {
        result = false;
      }
    }
  }

  return result;
}

bool Board::is_clear_horizontal(Square const& from, Square const& to)
{
  bool result = true;
  Square const* right = &to;
  Square const* left = &from;
  // Set up the counter for the while loop.
  // Assume we are going to start from one space ahead of the current square
  // and walk straight until we hit the destination square

  if (from.get_y() != to.get_y())
  {
    result = false;
  }
  else
  {
    // Set up the squares so we always walk up.
    // So if to is above from, swap them.
    if (to.get_x() < from.get_x())
    {
      left = &to;
      right = &from;
    }

    // Walk along the board and if we find an occupied space, exit the loop
    // and return false.
    for (int i = left->get_x() + 1; i < right->get_x() && result; i++)
    {
      if (square_at(i, from.get_y()).occupied())
      {
        result = false;
      }
    }
  }
  return result;
}

bool Board::is_clear_diagonal(Square const& from, Square const& to)
{
  bool result = true;

  // Assume that we are walking up
  int direction = 1;

  // Assume we are walking right
  Square const* left = &from;
  Square const* right = &to;

  if (std::abs(from.get_x() - to.get_x()) != std::abs(from.get_y() - to.get_y()))
  {
    result = false;
  }
  else
  {

    // Verify assumption that we are walking right
    if (from.get_x() > to.get_x())
    {
      left = &to;
      right = &from;
    }

    if (left->get_y() > right->get_y())
    {
      direction = -1;
    }

    // Walk from "left" to "right"
    for (int i = 1; i < right->get_x() - left->get_x(); i++)
    {
      // Check to see if square is occupied
      if (Board::get_board().square_at(left->get_x() + i, left->get_y() + direction * i).occupied())
      {
        result = false;
      }
    }
  }
  return result;
}

void Board::display(std::ostream& out)
{
  out << std::endl;
  for (int i = c_board_dimension - 1; i >= 0; i--)
  {
    out << (i + 1) << "  ";
    for (char j = 0; j < c_board_dimension; j++)
    {
      square_at(j, i).display(out);
    }
    out << std::endl << std::endl;
  }

  out << "   ";
  for (int i = 0; i < c_board_dimension; i++)
  {
    out << " " << (char)(i + 'A') << "  ";
  }
  std::cout << std::endl;
}
