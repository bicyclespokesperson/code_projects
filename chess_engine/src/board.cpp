#include "board.h"
#include "my_assert.h"

constexpr int c_board_dimension{8};

namespace
{
bool piece_can_move(Coordinates from, Coordinates to, Board const& board);

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
  if (board.distance_between(from, to) == 1)
  {
    return true;
  }

  // Check if castling is allowed
  if (from.y() == to.y() && std::abs(from.x() - to.x()) == 2)
  {
    if (!board.check_castling_rights(to))
    {
      return false; // One of the pieces necessary for castling has already moved
    }

    Coordinates transit_square{static_cast<int8_t>((from.x() + to.x()) / 2), from.y()};

    if (board.square_at(transit_square).is_occupied())
    {
      return false;
    }

    // Make sure the knight is out of the way when long castling
    if (to == Coordinates{2, 0} && board.square_at(Coordinates{1, 0}).is_occupied())
    {
      return false;
    }

    if (to == Coordinates{2, 7} && board.square_at(Coordinates{1, 7}).is_occupied())
    {
      return false;
    }

    auto const& opposing_pieces = board.opposing_pieces(from);
    if (std::any_of(opposing_pieces.cbegin(), opposing_pieces.cend(), [&](Coordinates piece)
                    {
                      return piece_can_move(piece, from, board) || 
                      piece_can_move(piece, transit_square, board) || 
                      piece_can_move(piece, to, board);
                    }))
    {
      return false; // Illegal to castle out of, through, or into check
    }

    return true;
  }

  
  return false;
}

bool pawn_can_move(Coordinates from, Coordinates to, Board const& board)
{
  bool const is_white = board.square_at(from).is_white();

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
    // En passent
    else if (move_distance == 1 &&
             board.is_clear_diagonal(from, to) &&
             board.previous_move() && board.previous_move()->first.x() == to.x() &&
             std::abs(board.previous_move()->first.y() - to.y()) == 1 &&
             std::abs(board.previous_move()->second.y() - to.y()) == 1 &&
             board.square_at(board.previous_move()->second).occupier() == Piece::pawn)
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
: m_white_king({0, 0}),
  m_black_king({0, 0})
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


  auto perform_move = [&](Coordinates from, Coordinates to)
  {
    auto& pieces = friendly_pieces(from);
    square_at(to).set_occupier(square_at(from).occupier());
    square_at(to).set_occupier_color(square_at(from).occupier_color());
    square_at(from).remove_occupier();

    //std::cout << "\nBefore update\n";
    //std::copy(pieces.begin(),pieces.end(), std::ostream_iterator<Coordinates>(std::cout, " "));

    pieces.erase(std::upper_bound(pieces.begin(), pieces.end(), from) - 1);
    pieces.insert(std::upper_bound(pieces.begin(), pieces.end(), to), to);

    //std::cout << "\nAfter update\n";
    //std::copy(pieces.begin(), pieces.end(), std::ostream_iterator<Coordinates>(std::cout, " "));
  };

  // En passent
  if (square_at(from).occupier() == Piece::pawn && is_clear_diagonal(from, to) && !square_at(to).is_occupied())
  {
    //TODO: update piece collection for captures
    square_at(previous_move()->second).remove_occupier();
  }
  update_castling_rights_(from);

  // Castling
  if (square_at(from).occupier() == Piece::king && distance_between(from, to) == 2)
  {
    auto rook_move = find_castling_rook_move_(to);
    perform_move(rook_move.first, rook_move.second);
  }

  perform_move(from, to);

  // Pawn promotion
  if (square_at(to).occupier() == Piece::pawn && (to.y() == 0 || to.y() == 7))
  {
    square_at(to).set_occupier(Piece::queen);
  }
  
  // TODO: Check for check

  if (auto sq = square_at(to); sq.occupier() == Piece::king)
  {
    if (sq.is_white())
    {
      m_white_king = to;
    }
    else
    {
      m_black_king = to;
    }
  }

  m_previous_move = std::pair{from, to};

  MY_ASSERT(validate_(), "Board is in an incorrect state after move");
  return true;
}

std::pair<Coordinates, Coordinates> Board::find_castling_rook_move_(Coordinates king_destination)
{
  if (king_destination == Coordinates{2, 0})
  {
    return {{0, 0}, {3, 0}};
  }
  if (king_destination == Coordinates{6, 0})
  {
    return {{7, 0}, {5, 0}};
  }
  if (king_destination == Coordinates{2, 7})
  {
    return {{0, 7}, {3, 7}};
  }
  if (king_destination == Coordinates{6, 7})
  {
    return {{7, 7}, {5, 7}};
  }

  MY_ASSERT(false, "Invalid king move");
  return {{0, 0}, {0, 0}};
}

bool Board::check_castling_rights(Coordinates to) const
{
  if (to == Coordinates{2, 0})
  {
    return m_white_can_long_castle;
  }

  if (to == Coordinates{6, 0})
  {
    return m_white_can_short_castle;
  }

  if (to == Coordinates{2, 7})
  {
    return m_black_can_long_castle;
  }

  if (to == Coordinates{6, 7})
  {
    return m_black_can_short_castle;
  }

  return false;
}

void Board::update_castling_rights_(Coordinates from)
{
  if (square_at(from).occupier() == Piece::king)
  {
    if (square_at(from).occupier_color() == Color::black)
    {
      m_black_can_short_castle = false;
      m_black_can_long_castle = false;
    }
    else
    {
      m_white_can_short_castle = false;
      m_white_can_long_castle = false;
    }
  }

  if (square_at(from).occupier() == Piece::rook)
  {
    if (from == Coordinates{0, 0})
    {
      m_white_can_long_castle = false;
    }
    else if (from == Coordinates{7, 0})
    {
      m_white_can_short_castle = false;
    }
    else if (from == Coordinates{0, 7})
    {
      m_black_can_long_castle = false;
    }
    else if (from == Coordinates{7, 7})
    {
      m_black_can_short_castle = false;
    }
  }
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
  m_white_pieces.clear();
  m_black_pieces.clear();
  m_white_pieces.reserve(c_initial_piece_count);
  m_black_pieces.reserve(c_initial_piece_count);

  // White pieces
  square_at({0, 0}).set_occupier(Piece::rook);
  square_at({1, 0}).set_occupier(Piece::knight);
  square_at({2, 0}).set_occupier(Piece::bishop_dark);
  square_at({3, 0}).set_occupier(Piece::queen);
  square_at({4, 0}).set_occupier(Piece::king);
  square_at({5, 0}).set_occupier(Piece::bishop_light);
  square_at({6, 0}).set_occupier(Piece::knight);
  square_at({7, 0}).set_occupier(Piece::rook);
  for (int8_t i{0}; i < c_board_dimension; ++i)
  {
    square_at({i, 0}).set_occupier_color(Color::white);

    square_at({i, 1}).set_occupier(Piece::pawn);
    square_at({i, 1}).set_occupier_color(Color::white);

    m_white_pieces.push_back({i, 0});
    m_white_pieces.push_back({i, 1});
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
  for (int8_t i{0}; i < c_board_dimension; ++i)
  {
    square_at({i, 7}).set_occupier_color(Color::black);

    square_at({i, 6}).set_occupier(Piece::pawn);
    square_at({i, 6}).set_occupier_color(Color::black);

    m_black_pieces.push_back({i, 7});
    m_black_pieces.push_back({i, 6});
  }

  std::sort(m_white_pieces.begin(), m_white_pieces.end());
  std::sort(m_black_pieces.begin(), m_black_pieces.end());

  m_white_king = {4, 0};
  m_black_king = {4, 7};
  m_white_can_short_castle = true;
  m_white_can_long_castle = true;
  m_black_can_short_castle = true;
  m_black_can_long_castle = true;
  m_previous_move = {};

  MY_ASSERT(validate_(), "Board is in an incorrect state");
}

Square const& Board::square_at(Coordinates coords) const
{
  return m_squares.at(c_board_dimension * coords.x() + coords.y());
}

Square& Board::square_at(Coordinates coords)
{
  return const_cast<Square&>(std::as_const(*this).square_at(coords));
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
  for (int8_t i = bottom.y() + 1; i < top.y(); i++)
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
  for (int8_t i = left->x() + 1; i < right->x(); i++)
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
    if (square_at({static_cast<int8_t>(left->x() + i), static_cast<int8_t>(left->y() + direction * i)}).is_occupied())
    {
      return false;
    }
  }
  return true;
}

std::optional<std::pair<Coordinates, Coordinates>> Board::previous_move() const
{
  return m_previous_move;
}

std::vector<Coordinates> const& Board::opposing_pieces(Coordinates piece_location) const
{
  if (square_at(piece_location).is_white())
  {
    return m_black_pieces;
  }
  return m_white_pieces;
}

std::vector<Coordinates>& Board::friendly_pieces(Coordinates piece_location)
{
  if (square_at(piece_location).is_white())
  {
    return m_white_pieces;
  }
  return m_black_pieces;
}

bool Board::is_in_check_(Color color) const
{
  auto king_location = (color == Color::white) ? m_white_king : m_black_king;
  auto const& pieces = opposing_pieces(king_location);

  return std::any_of(pieces.cbegin(), pieces.cend(), [&](Coordinates piece_location)
                     {
                       return piece_can_move(piece_location, king_location, *this);
                     });
}

bool Board::validate_() const
{
  // Count how many pieces of each color are on the board
  auto verify_pieces = [&](auto color, auto const& pieces)
  {
    auto const pieces_count = std::count_if(m_squares.cbegin(), m_squares.cend(), [&](auto const& square)
                                            {
                                              return square.is_occupied() && square.occupier_color() == color;
                                            });

    if (static_cast<size_t>(pieces_count) != pieces.size())
    {
      std::cerr << "Incorrect piece count for " << ((color == Color::black) ? "black" : "white") << ": " << pieces_count << "!=" << pieces.size() << std::endl;
      return false;
    }

    for (auto const& coord : pieces)
    {
      if (!square_at(coord).is_occupied() || !(square_at(coord).occupier_color() == color))
      {
        std::cerr << "Square {" << std::to_string(coord.x()) << ", " << std::to_string(coord.y()) << "} has incorrect color, expected: " << 
          ((color == Color::black) ? "black" : "white") << ", occupier: " << std::to_string(static_cast<uint8_t>(square_at(coord).occupier())) << std::endl;
        return false;
      }
    }

    if (!std::is_sorted(pieces.cbegin(), pieces.cend()))
    {
      std::cerr << "Pieces are not sorted for " << ((color == Color::black) ? "black" : "white") << std::endl;
      return false;
    }

    if (std::adjacent_find(pieces.cbegin(), pieces.cend()) != pieces.cend())
    {
      std::cerr << "Pieces are not unique for " << ((color == Color::black) ? "black" : "white") << std::endl;
      return false;
    }

    return true;
  };

  if (auto sq = square_at(m_white_king); 
      sq.occupier() != Piece::king || !sq.is_white())
  {
    std::cerr << "White king not found at " << m_white_king << std::endl;
    return false;
  }
  if (auto sq = square_at(m_black_king); 
      sq.occupier() != Piece::king || sq.is_white())
  {
    std::cerr << "Black king not found at " << m_black_king << std::endl;
    return false;
  }

  return verify_pieces(Color::black, m_black_pieces) && verify_pieces(Color::white, m_white_pieces);
}

void Board::display(std::ostream& out) const
{
  out << std::endl;
  for (int i = c_board_dimension - 1; i >= 0; i--)
  {
    out << (i + 1) << "  ";
    for (int j = 0; j < c_board_dimension; j++)
    {
      square_at({static_cast<int8_t>(j), static_cast<int8_t>(i)}).display(out);
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

