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
  if (!(board.is_clear_diagonal(from, to) || board.is_clear_horizontal(from, to) || board.is_clear_vertical(from, to)))
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

    auto const& opposing_pieces = board.get_opposing_pieces(from);
    if (std::any_of(opposing_pieces.cbegin(), opposing_pieces.cend(), [&](Coordinates piece) {
          return piece_can_move(piece, from, board) || piece_can_move(piece, transit_square, board) ||
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
  int const max_distance = [&] {
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
  if ((is_white && to.y() > from.y()) || (!is_white && to.y() < from.y()))
  {
    // Ensure the space ahead of the pawn is clear and vertical
    // This also prevents moves of zero spaces, since it cannot move to
    // a space occupied by itself.
    if (board.is_clear_vertical(from, to) && !(board.square_at(to).is_occupied()))
    {
      return true;
    }
    // If the square is diagonally forward and occupied by an opponent
    else if (move_distance == 1 && board.is_clear_diagonal(from, to) && board.square_at(to).is_occupied())
    {
      return true;
    }
    // En passent
    else if (move_distance == 1 && board.is_clear_diagonal(from, to) && board.previous_move() &&
             board.previous_move()->from.x() == to.x() && 
             std::abs(board.previous_move()->from.y() - to.y()) == 1 &&
             std::abs(board.previous_move()->to.y() - to.y()) == 1 &&
             board.square_at(board.previous_move()->to).occupier() == Piece::pawn)
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
  case Piece::pawn: {
    auto result = pawn_can_move(from, to, board);
    return result;
  }
  case Piece::knight:
    return knight_can_move(from, to, board);
  case Piece::bishop:
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

} // namespace

Board::Board() : m_white_king({0, 0}), m_black_king({0, 0})
{
  static_assert(sizeof(Square) == 1);

  setup();
}

Board::~Board() = default;

std::optional<std::pair<Coordinates, Piece>> Board::perform_move_(Coordinates from, Coordinates to,
                                                                  Coordinates capture_location)
{
  std::optional<std::pair<Coordinates, Piece>> captured_piece;
  auto& pieces = get_friendly_pieces(from);
  auto& opposing_pieces = get_opposing_pieces(from);

  if (square_at(to).is_occupied())
  {
    captured_piece = std::pair{to, square_at(capture_location).occupier()};
    remove_piece_(opposing_pieces, capture_location);
    square_at(capture_location).remove_occupier();
  }

  square_at(to).set_occupier(square_at(from).occupier());
  square_at(to).set_occupier_color(square_at(from).occupier_color());
  square_at(from).remove_occupier();

  remove_piece_(pieces, from);
  add_piece_(pieces, to);

  update_king_locations_(to);

  return captured_piece;
}

void Board::unperform_move_(Coordinates from, Coordinates to,
                            std::optional<std::pair<Coordinates, Piece>> captured_piece)
{
  MY_ASSERT(square_at(to).is_occupied() && !square_at(from).is_occupied(),
            "This function can only be called after a move has been performed");

  auto& pieces = get_friendly_pieces(to);
  auto& opposing_pieces = get_opposing_pieces(to);
  bool white_move = square_at(to).is_white();

  remove_piece_(pieces, to);
  add_piece_(pieces, from);

  square_at(from).set_occupier(square_at(to).occupier());
  square_at(from).set_occupier_color(square_at(to).occupier_color());
  square_at(to).remove_occupier();

  if (captured_piece)
  {
    add_piece_(opposing_pieces, captured_piece->first);
    square_at(captured_piece->first).set_occupier(captured_piece->second);
    square_at(captured_piece->first).set_occupier_color((white_move) ? Color::black : Color::white);
  }

  update_king_locations_(from);
}

bool Board::try_make_move(Board::Move m)
{
  auto piece_to_move = square_at(m.from).occupier();
  if (piece_to_move == Piece::empty)
  {
    return false;
  }

  if (!previous_move() && !square_at(m.from).is_white())
  {
    // White goes first
    return false;
  }

  if (previous_move() && square_at(m.from).occupier_color() == square_at(previous_move()->to).occupier_color())
  {
    // Can't move the same color twice in a row
    return false;
  }

  if (!piece_can_move(m.from, m.to, *this))
  {
    return false;
  }

  auto capture_location = m.to;
  if (square_at(m.from).occupier() == Piece::pawn && is_clear_diagonal(m.from, m.to) && !square_at(m.to).is_occupied())
  {
    // Update capture location for En passent case
    capture_location = previous_move()->to;
  }

  auto captured_piece = perform_move_(m.from, m.to, capture_location);
  if (is_in_check_(square_at(m.to).occupier_color()))
  {
    unperform_move_(m.from, m.to, captured_piece);
    return false;
  }

  update_castling_rights_(m.to);

  // Move rook if the move was a castle
  if (square_at(m.to).occupier() == Piece::king && distance_between(m.from, m.to) == 2)
  {
    auto rook_move = find_castling_rook_move_(m.to);
    perform_move_(rook_move.from, rook_move.to, rook_move.to);
  }

  // Promote pawn to queen if it reached the last rank
  if (square_at(m.to).occupier() == Piece::pawn && (m.to.y() == 0 || m.to.y() == 7))
  {
    square_at(m.to).set_occupier(Piece::queen);
  }

  m_previous_move = Board::Move{m.from, m.to};

  MY_ASSERT(validate_(), "Board is in an incorrect state after move");
  return true;
}

Board::Move Board::find_castling_rook_move_(Coordinates king_destination)
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

  MY_ASSERT(false, "Invalid castling move");
  return {{0, 0}, {0, 0}};
}

bool Board::check_castling_rights(Coordinates dest) const
{
  if (dest == Coordinates{2, 0})
  {
    return m_white_can_long_castle;
  }

  if (dest == Coordinates{6, 0})
  {
    return m_white_can_short_castle;
  }

  if (dest == Coordinates{2, 7})
  {
    return m_black_can_long_castle;
  }

  if (dest == Coordinates{6, 7})
  {
    return m_black_can_short_castle;
  }

  return false;
}

void Board::update_king_locations_(Coordinates dest)
{
  if (auto sq = square_at(dest); sq.occupier() == Piece::king)
  {
    if (sq.is_white())
    {
      m_white_king = dest;
    }
    else
    {
      m_black_king = dest;
    }
  }
}

void Board::update_castling_rights_(Coordinates dest)
{
  if (square_at(dest).occupier() == Piece::king)
  {
    if (square_at(dest).occupier_color() == Color::black)
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

  if (square_at(dest).occupier() == Piece::rook)
  {
    if (dest == Coordinates{0, 0})
    {
      m_white_can_long_castle = false;
    }
    else if (dest == Coordinates{7, 0})
    {
      m_white_can_short_castle = false;
    }
    else if (dest == Coordinates{0, 7})
    {
      m_black_can_long_castle = false;
    }
    else if (dest == Coordinates{7, 7})
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
  m_white_piece_locations.clear();
  m_black_piece_locations.clear();
  m_white_piece_locations.reserve(c_initial_piece_count);
  m_black_piece_locations.reserve(c_initial_piece_count);

  // White pieces
  square_at({0, 0}).set_occupier(Piece::rook);
  square_at({1, 0}).set_occupier(Piece::knight);
  square_at({2, 0}).set_occupier(Piece::bishop);
  square_at({3, 0}).set_occupier(Piece::queen);
  square_at({4, 0}).set_occupier(Piece::king);
  square_at({5, 0}).set_occupier(Piece::bishop);
  square_at({6, 0}).set_occupier(Piece::knight);
  square_at({7, 0}).set_occupier(Piece::rook);
  for (int8_t i{0}; i < c_board_dimension; ++i)
  {
    square_at({i, 0}).set_occupier_color(Color::white);

    square_at({i, 1}).set_occupier(Piece::pawn);
    square_at({i, 1}).set_occupier_color(Color::white);

    m_white_piece_locations.push_back({i, 0});
    m_white_piece_locations.push_back({i, 1});
  }

  // Black pieces
  square_at({0, 7}).set_occupier(Piece::rook);
  square_at({1, 7}).set_occupier(Piece::knight);
  square_at({2, 7}).set_occupier(Piece::bishop);
  square_at({3, 7}).set_occupier(Piece::queen);
  square_at({4, 7}).set_occupier(Piece::king);
  square_at({5, 7}).set_occupier(Piece::bishop);
  square_at({6, 7}).set_occupier(Piece::knight);
  square_at({7, 7}).set_occupier(Piece::rook);
  for (int8_t i{0}; i < c_board_dimension; ++i)
  {
    square_at({i, 7}).set_occupier_color(Color::black);

    square_at({i, 6}).set_occupier(Piece::pawn);
    square_at({i, 6}).set_occupier_color(Color::black);

    m_black_piece_locations.push_back({i, 7});
    m_black_piece_locations.push_back({i, 6});
  }

  std::sort(m_white_piece_locations.begin(), m_white_piece_locations.end());
  std::sort(m_black_piece_locations.begin(), m_black_piece_locations.end());

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
  auto right = to;
  auto left = from;
  if (to.x() < from.x())
  {
    std::swap(left, right);
  }

  // Walk along the board and if we find an occupied space, exit the loop
  // and return false.
  for (int8_t i = left.x() + 1; i < right.x(); i++)
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
  auto left = from;
  auto right = to;
  if (from.x() > to.x())
  {
    std::swap(left, right);
  }

  // Assume that we are walking up
  int8_t direction = 1;
  if (left.y() > right.y())
  {
    direction = -1;
  }

  // Walk from "left" to "right"
  for (int8_t i = 1; i < right.x() - left.x(); i++)
  {
    // Check to see if square is occupied
    if (square_at({static_cast<int8_t>(left.x() + i), static_cast<int8_t>(left.y() + direction * i)}).is_occupied())
    {
      return false;
    }
  }
  return true;
}

std::optional<Board::Move> Board::previous_move() const
{
  return m_previous_move;
}

std::vector<Coordinates> const& Board::get_opposing_pieces(Coordinates piece_location) const
{
  if (square_at(piece_location).is_white())
  {
    return m_black_piece_locations;
  }
  return m_white_piece_locations;
}

std::vector<Coordinates> const& Board::get_friendly_pieces(Coordinates piece_location) const
{
  if (square_at(piece_location).is_white())
  {
    return m_white_piece_locations;
  }
  return m_black_piece_locations;
}

std::vector<Coordinates> const& Board::get_pieces(Color color) const
{
  if (color == Color::white)
  {
    return m_white_piece_locations;
  }
  return m_black_piece_locations;
}

std::vector<Coordinates>& Board::get_opposing_pieces(Coordinates piece_location)
{
  return const_cast<std::vector<Coordinates>&>(std::as_const(*this).get_opposing_pieces(piece_location));
}

std::vector<Coordinates>& Board::get_friendly_pieces(Coordinates piece_location)
{
  return const_cast<std::vector<Coordinates>&>(std::as_const(*this).get_friendly_pieces(piece_location));
}

std::vector<Coordinates>& Board::get_pieces(Color color)
{
  return const_cast<std::vector<Coordinates>&>(std::as_const(*this).get_pieces(color));
}

bool Board::is_in_check_(Color color) const
{
  auto king_location = (color == Color::white) ? m_white_king : m_black_king;
  auto const& pieces = get_opposing_pieces(king_location);

  return std::any_of(pieces.cbegin(), pieces.cend(), [&](Coordinates piece_location) {
    return piece_can_move(piece_location, king_location, *this);
  });
}

void Board::remove_piece_(std::vector<Coordinates>& piece_locations, Coordinates to_remove)
{
  MY_ASSERT(std::binary_search(piece_locations.cbegin(), piece_locations.cend(), to_remove),
            "Cannot remove piece that is not present");
  piece_locations.erase(std::upper_bound(piece_locations.begin(), piece_locations.end(), to_remove) - 1);
}

void Board::add_piece_(std::vector<Coordinates>& piece_locations, Coordinates to_add)
{
  MY_ASSERT(!std::binary_search(piece_locations.cbegin(), piece_locations.cend(), to_add),
            "Cannot add piece that is already present");
  piece_locations.insert(std::upper_bound(piece_locations.begin(), piece_locations.end(), to_add), to_add);
}

bool Board::validate_() const
{
  // Count how many pieces of each color are on the board
  auto verify_pieces = [&](auto color, auto const& pieces) {
    auto const pieces_count = std::count_if(m_squares.cbegin(), m_squares.cend(), [&](auto const& square) {
      return square.is_occupied() && square.occupier_color() == color;
    });

    if (static_cast<size_t>(pieces_count) != pieces.size())
    {
      std::cerr << "Incorrect piece count for " << ((color == Color::black) ? "black" : "white") << ": " << pieces_count
                << "!=" << pieces.size() << std::endl;
      return false;
    }

    for (auto const& coord : pieces)
    {
      if (!square_at(coord).is_occupied() || !(square_at(coord).occupier_color() == color))
      {
        std::cerr << "Square {" << std::to_string(coord.x()) << ", " << std::to_string(coord.y())
                  << "} has incorrect color, expected: " << ((color == Color::black) ? "black" : "white")
                  << ", occupier: " << std::to_string(static_cast<uint8_t>(square_at(coord).occupier())) << std::endl;
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

  if (auto sq = square_at(m_white_king); sq.occupier() != Piece::king || !sq.is_white())
  {
    std::cerr << "White king not found at " << m_white_king << std::endl;
    return false;
  }
  if (auto sq = square_at(m_black_king); sq.occupier() != Piece::king || sq.is_white())
  {
    std::cerr << "Black king not found at " << m_black_king << std::endl;
    return false;
  }

  return verify_pieces(Color::black, m_black_piece_locations) && verify_pieces(Color::white, m_white_piece_locations);
}

std::vector<Coordinates> Board::find_piece(Piece piece, Color color, Coordinates target_square) const
{
  std::vector<Coordinates> candidates;
  auto const& locations = get_pieces(color);
  std::copy_if(locations.cbegin(), locations.cend(), std::back_inserter(candidates), [&](Coordinates piece_loc) {
    return square_at(piece_loc).occupier() == piece && piece_can_move(piece_loc, target_square, *this);
  });

  return candidates;
}

std::optional<Board::Move> move_from_pgn(std::string_view move_param, Board const& board,
                                                              Color color)
{
  std::string move_str{move_param};
  move_str.erase(std::remove_if(move_str.begin(), move_str.end(),
                                [chars = std::string("x+#?!")](char c) {
                                  return isspace(c) || (chars.find(c) != std::string::npos);
                                }),
                 move_str.end());

  if (move_str == "O-O" || move_str == "0-0")
  {
    if (color == Color::white)
    {
      return Board::Move{Coordinates{4, 0}, Coordinates{6, 0}};
    }
    else
    {
      return Board::Move{Coordinates{4, 7}, Coordinates{6, 7}};
    }
  }

  if (move_str == "O-O-O" || move_str == "0-0-0")
  {
    if (color == Color::white)
    {
      return Board::Move{Coordinates{4, 0}, Coordinates{2, 0}};
    }
    else
    {
      return Board::Move{Coordinates{4, 7}, Coordinates{2, 7}};
    }
  }

  Coordinates target_square{static_cast<int8_t>(toupper(move_str[move_str.size() - 2]) - 'A'),
                            static_cast<int8_t>(move_str[move_str.size() - 1] - '1')};

  move_str.resize(move_str.size() - 2); // Drop target square from string
  auto const piece = [&] {
    // Handle pawn move case (ex: e4, xe4, fxe4)
    if (move_str.empty() || islower(move_str[0]))
    {
      return Piece::pawn;
    }

    // Handle piece move case (ex: Bc4)
    auto piece = from_char(move_str[0]);
    move_str = move_str.substr(1);

    return piece;
  }();

  if (piece == Piece::empty)
  {
    std::cerr << "Invalid move: " << move_param << "\n";
    return {};
  }

  auto candidates = board.find_piece(piece, color, target_square);
  if (candidates.empty())
  {
    std::cerr << "No piece can perform move " << move_param << "\n";
    return {};
  }

  if (candidates.size() == 1)
  {
    // Exactly one piece can move to the target square
    return Board::Move{candidates.front(), target_square};
  }

  if (move_str.empty())
  {
    std::cerr << "Too many pieces can perform move " << move_param << "\n";
    return {};
  }

  if (isalpha(move_str[0]))
  {
    // Drop candidates that are not on the correct column
    auto start_column = static_cast<int8_t>(move_str[0] - 'a');
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                    [start_column](Coordinates piece_loc) {
                                      return piece_loc.x() != start_column;
                                    }),
                     candidates.end());

    move_str = move_str.substr(1);
    if (candidates.size() == 1)
    {
      // Exactly one piece can move to the target square
      return Board::Move{candidates.front(), target_square};
    }
  }

  if (move_str.empty())
  {
    std::cerr << "Too many pieces can perform move " << move_param << "\n";
    return {};
  }

  if (isdigit(move_str[0]))
  {

    // Drop candidates that are not on the correct column
    auto start_row = static_cast<int8_t>(move_str[0] - '1');
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                    [start_row](Coordinates piece_loc) {
                                      return piece_loc.y() != start_row;
                                    }),
                     candidates.end());
    move_str = move_str.substr(1);
    if (candidates.size() == 1)
    {
      // Exactly one piece can move to the target square
      return Board::Move{candidates.front(), target_square};
    }
  }

  if (candidates.size() > 1)
  {
    std::cerr << "Too many pieces can perform move " << move_param << "\n";
  }
  else
  {
    std::cerr << "No piece can perform move " << move_param << "\n";
  }
  return {};
}

std::optional<Board> Board::from_pgn(std::string_view pgn)
{
  std::vector<std::string> tag_pairs;
  std::vector<std::string> moves;

  size_t index{0};
  std::string game_result;

  while (index < pgn.size() && index != std::string::npos)
  {
    // Skip to the next non-space character, starting at the first character we haven't seen yet
    if (index != 0)
    {
      ++index;
    }
    index = pgn.find_first_not_of(" \t\n", index);

    if (pgn[index] == '[')
    {
      auto end_index = pgn.find(']', index);
      tag_pairs.emplace_back(pgn.substr(index + 1, end_index - index - 1)); // Drop the [] in the tag pair
      index = end_index;
    }
    else if (isalpha(pgn[index]) || pgn[index] == '0') // Castling uses both 'O and '0'
    {
      auto end_index = pgn.find_first_of(" \t\n", index);
      moves.emplace_back(pgn.substr(index, end_index - index));
      index = end_index;
    }
    else if (isdigit(pgn[index])) // Skip move number
    {
      auto end_index = pgn.find('.', index);
      if (end_index == std::string::npos)
      {
        game_result = pgn.substr(index, end_index - index);
      }
      index = end_index;
    }
    else if (pgn[index] == ';') // Skip comments
    {
      index = pgn.find('\n', index);
    }
    else if (pgn[index] == '{') // Skip comments
    {
      index = pgn.find('}', index);
    }
    else
    {
      std::cerr << "Unexpected char at index " << std::to_string(index) << " while parsing pgn file: " << pgn[index]
                << "Ascii code: " << std::to_string(static_cast<int32_t>(pgn[index])) << std::endl;
      return {};
    }
  }

  std::optional<Board> result = Board{};
  auto color{Color::white};
  for (auto const& move_str : moves)
  {
    auto move_to_try = move_from_pgn(move_str, *result, color);
    if (move_to_try && result->try_make_move(*move_to_try))
    {
      std::cout << "Move " << move_str << ": " << move_to_try->from << " -> " << move_to_try->to << std::endl;
    }
    else
    {
      result->display(std::cout);
      return {};
    }
    color = (color == Color::white) ? Color::black : Color::white;
  }

  return result;
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
  out << std::endl;
}

void display_piece_locations_(std::vector<Coordinates> const& piece_locations)
{
  std::copy(piece_locations.cbegin(), piece_locations.cend(), std::ostream_iterator<Coordinates>(std::cerr, " "));
}
