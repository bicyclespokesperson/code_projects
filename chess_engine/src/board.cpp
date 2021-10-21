#include "board.h"

constexpr int c_board_dimension{8};

namespace
{
bool occupied_by_friend(Coordinates from, Coordinates to, Board const& board)
{
  if (auto const& square = board.square_at(to); 
      square.is_occupied() && square.occupier_color() == board.square_at(from).occupier_color())
  {
    return true;
  }
  return false;
}

bool bishop_can_move(Coordinates from, Coordinates to, Board const& board)
{
  if (!(board.is_clear_diagonal(from, to)))
  {
    return false;
  }

  return true;
}

bool knight_can_move(Coordinates from, Coordinates to, Board const& /* board */)
{
  // Make sure the move is either two vertical and one horizontal
  if (std::abs(from.y() - to.y()) == 2 && std::abs(from.x() - to.x()) == 1)
  {
    return true;
  }

  // Or two horizontal and one vertical
  else if (std::abs(from.x() - to.x()) == 2 && std::abs(from.y() - to.y()) == 1)
  {
    return true;
  }

  return false;
}

bool rook_can_move(Coordinates from, Coordinates to, Board const& board)
{
  if (!(board.is_clear_horizontal(from, to) || board.is_clear_vertical(from, to)))
  {
    return false;
  }

  return true;
}

bool queen_can_move(Coordinates from, Coordinates to, Board const& board)
{
  bool result = true;

  // If the square is not horizontally, vertically, or diagonally away from the
  // Queen's current location, the move is invalid.
  if (!(board.is_clear_diagonal(from, to) ||
        board.is_clear_horizontal(from, to) ||
        board.is_clear_vertical(from, to)))
  {
    result = false;
  }

  return result;
}

bool king_can_move(Coordinates from, Coordinates to, Board const& board)
{
  //TODO: Support Castling

  if (board.distance_between(from, to) != 1)
  {
    return false;
  }

  return true;
}

bool pawn_can_move(Coordinates from, Coordinates to, Board const& board)
{
  bool const is_white = board.square_at(from).occupier_color() != Color::black;

  // A pawn can move two spaces on its first move
  int const max_distance = [&]
  {
    if ((is_white && from.y() == 1) || (!is_white && from.y() == 6))
    {
      return 2;
    }
    return 1;
  }();

  // Make sure the distance of the move is not greater than 1, or 2 if the
  // piece has not yet moved.
  int const move_distance = board.distance_between(from, to);
  if (move_distance > max_distance)
  {
    return false;
  }

  // Make sure the pawn is moving forward
  if ((is_white && to.y() > from.y()) ||
      (!is_white && to.y() < from.y()))
  {
    // Ensure the space ahead of the pawn is clear and vertical
    // This also prevents moves of zero spaces, since it cannot move to
    // a space occupied by itself.
    if (board.is_clear_vertical(from, to) &&
        !(board.square_at(to).is_occupied()))
    {
      return true;
    }
    // If the square is diagonally forward and occupied by an opponent
    else if (move_distance == 1 &&
             board.is_clear_diagonal(from, to) &&
             board.square_at(to).is_occupied())
    {
      return true;
    }
  }

  return false;
}

bool piece_can_move(Coordinates from, Coordinates to, Board const& board)
{
  if (!board.square_at(from).is_occupied())
  {
    return false;
  }

  if (occupied_by_friend(from, to, board))
  {
    return false;
  }

  switch (board.square_at(from).occupier())
  {
    case Piece::pawn:
      return pawn_can_move(from, to, board);
    case Piece::knight:
      return knight_can_move(from, to, board);
    case Piece::bishop_dark:
      [[fallthrough]];
    case Piece::bishop_light:
      return bishop_can_move(from, to, board);
    case Piece::rook:
      return rook_can_move(from, to, board);
    case Piece::queen:
      return queen_can_move(from, to, board);
    case Piece::king:
      return king_can_move(from, to, board);
    default:
      return false;
  }
}

}

Board::Board()
{
  static_assert(sizeof(Square) == 1);

  setup();
}

Board::~Board() = default;

bool Board::make_move(Coordinates from, Coordinates to)
{
  auto piece_to_move = square_at(from).occupier();
  if (piece_to_move == Piece::empty)
  {
    return false;
  }

  if (!piece_can_move(from, to, *this))
  {
    return false;
  }

  // TODO: Check for check
  
  //TODO: Add/update list of pieces per side so we can check for check
  square_at(to).set_occupier(square_at(from).occupier());
  square_at(from).set_occupier(Piece::empty);


  return true;
}

/**
 * Calculates the distance between two squares on the board
 * @param from The first square
 * @param to The second square
 * @return The resulting distance, or -1 if there is no line between the two squares
 */
int Board::distance_between(Coordinates from, Coordinates to) const
{
  // If the squares are on the same vertical, return the difference
  // in their horizontals
  if (from.x() == to.x())
  {
    return std::abs(from.y() - to.y());
  }

  // If the squares are on the same horizontal, return the difference
  // in their verticals
  else if (from.y() == to.y())
  {
    return std::abs(from.x() - to.x());
  }

  // If the squares are on the same diagonal, their x and y differences
  // will be equal, so return their X difference
  else if (std::abs(from.x() - to.x()) == std::abs(from.y() - to.y()))
  {
    return std::abs(from.x() - to.x());
  }

  return -1;
}

void Board::setup()
{
  // White pieces
  square_at({0, 0}).set_occupier(Piece::rook);
  square_at({1, 0}).set_occupier(Piece::knight);
  square_at({2, 0}).set_occupier(Piece::bishop_dark);
  square_at({3, 0}).set_occupier(Piece::queen);
  square_at({4, 0}).set_occupier(Piece::king);
  square_at({5, 0}).set_occupier(Piece::bishop_light);
  square_at({6, 0}).set_occupier(Piece::knight);
  square_at({7, 0}).set_occupier(Piece::rook);
  for (uint8_t i{0}; i < 8; ++i)
  {
    square_at({i, 0}).set_occupier_color(Color::white);

    square_at({i, 1}).set_occupier(Piece::pawn);
    square_at({i, 1}).set_occupier_color(Color::white);
  }

  // Black pieces
  square_at({0, 7}).set_occupier(Piece::rook);
  square_at({1, 7}).set_occupier(Piece::knight);
  square_at({2, 7}).set_occupier(Piece::bishop_dark);
  square_at({3, 7}).set_occupier(Piece::queen);
  square_at({4, 7}).set_occupier(Piece::king);
  square_at({5, 7}).set_occupier(Piece::bishop_light);
  square_at({6, 7}).set_occupier(Piece::knight);
  square_at({7, 7}).set_occupier(Piece::rook);
  for (uint8_t i{0}; i < 8; ++i)
  {
    square_at({i, 7}).set_occupier_color(Color::black);

    square_at({i, 6}).set_occupier(Piece::pawn);
    square_at({i, 6}).set_occupier_color(Color::black);
  }
}

Square Board::square_at(Coordinates coords) const
{
  return m_squares.at(c_board_dimension * coords.x() + coords.y());
}

Square& Board::square_at(Coordinates coords)
{
  return m_squares.at(c_board_dimension * coords.x() + coords.y());
}

bool Board::is_clear_vertical(Coordinates from, Coordinates to) const
{
  // Set up the counter for the while loop.
  // Assume we are going to start from one space ahead of the current square
  // and walk straight until we hit the destination square

  if (from.x() != to.x())
  {
    return false;
  }
  
  auto top = to;
  auto bottom = from;
  // Set up the squares so we always walk up.
  // So if "to" is above "from", swap them.
  if (to.y() < from.y())
  {
    std::swap(bottom, top);
  }

  // Walk along the board and if we find an occupied space, exit the loop
  // and return false.
  for (uint8_t i = bottom.y() + 1; i < top.y(); i++)
  {
    if (square_at({from.x(), i}).is_occupied())
    {
      return false;
    }
  }

  return true;
}

bool Board::is_clear_horizontal(Coordinates from, Coordinates to) const
{
  // Set up the counter for the while loop.
  // Assume we are going to start from one space ahead of the current square
  // and walk straight until we hit the destination square

  if (from.y() != to.y())
  {
    return false;
  }
  
  // Set up the squares so we always walk up.
  // So if to is above from, swap them.
  auto right = &to;
  auto left = &from;
  if (to.x() < from.x())
  {
    std::swap(left, right);
  }

  // Walk along the board and if we find an occupied space, exit the loop
  // and return false.
  for (uint8_t i = left->x() + 1; i < right->x(); i++)
  {
    if (square_at({i, from.y()}).is_occupied())
    {
      return false;
    }
  }
  return true;
}

bool Board::is_clear_diagonal(Coordinates from, Coordinates to) const
{
  if (std::abs(from.x() - to.x()) != std::abs(from.y() - to.y()))
  {
    return false;
  }
  
  // Ensure we are walking right
  auto left = &from;
  auto right = &to;
  if (from.x() > to.x())
  {
    std::swap(left, right);
  }

  // Assume that we are walking up
  int8_t direction = 1;
  if (left->y() > right->y())
  {
    direction = -1;
  }

  // Walk from "left" to "right"
  for (int8_t i = 1; i < right->x() - left->x(); i++)
  {
    // Check to see if square is occupied
    if (square_at({static_cast<uint8_t>(left->x() + i), static_cast<uint8_t>(left->y() + direction * i)}).is_occupied())
    {
      return false;
    }
  }
  return true;
}

void Board::display(std::ostream& out) const
{
  out << std::endl;
  for (int i = c_board_dimension - 1; i >= 0; i--)
  {
    out << (i + 1) << "  ";
    for (int j = 0; j < c_board_dimension; j++)
    {
      square_at({static_cast<uint8_t>(j), static_cast<uint8_t>(i)}).display(out);
    }
    out << std::endl << std::endl;
  }

  out << "   ";
  for (int i = 0; i < c_board_dimension; i++)
  {
    out << " " << static_cast<char>(i + 'A') << "  ";
  }
  std::cout << std::endl;
}

