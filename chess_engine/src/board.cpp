#include "board.h"
#include "move_generator.h"
#include "my_assert.h"

namespace
{
bool piece_can_move(Coordinates from, Coordinates to, Board const& board);

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

    Coordinates transit_square{static_cast<int32_t>((from.x() + to.x()) / 2), from.y()};

    if (board.is_occupied(transit_square))
    {
      return false;
    }

    // Make sure the knight is out of the way when long castling
    if (to == Coordinates{2, 0} && board.is_occupied(Coordinates{1, 0}))
    {
      return false;
    }

    if (to == Coordinates{2, 7} && board.is_occupied(Coordinates{1, 7}))
    {
      return false;
    }

    auto const color = board.get_piece_color(from);
    auto attacked_squares = Move_generator::get_all_attacked_squares(board, opposite_color(color));
    if (attacked_squares.is_set(from) || attacked_squares.is_set(to) || attacked_squares.is_set(transit_square))
    {
      return false; // Illegal to castle out of, through, or into check
    }

    return true;
  }

  return false;
}

bool pawn_can_move(Coordinates from, Coordinates to, Board const& board)
{
  auto const color = board.get_piece_color(from);
  bool const is_white = color == Color::white;

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
  if ((is_white && to.y() > from.y()) || (!is_white && to.y() < from.y()))
  {
    // Ensure the space ahead of the pawn is clear and vertical
    // This also prevents moves of zero spaces, since it cannot move to
    // a space occupied by itself.
    if (board.is_clear_vertical(from, to) && !(board.is_occupied(to)))
    {
      return true;
    }
    // If the square is diagonally forward and occupied by an opponent
    else if (move_distance == 1 && board.is_clear_diagonal(from, to) && board.is_occupied(to))
    {
      return true;
    }
    // En passant
    else if (move_distance == 1 && board.is_clear_diagonal(from, to) && board.get_en_passant_square().is_set(to))
    {
      return true;
    }
  }

  return false;
}

bool piece_can_move(Coordinates from, Coordinates to, Board const& board)
{
  if (!board.is_occupied(from))
  {
    return false;
  }

  auto const color = board.get_piece_color(from);

  if (board.get_all(color).is_set(to))
  {
    return false;
  }

  switch (board.get_piece(from))
  {
    case Piece::pawn:
      return pawn_can_move(from, to, board);
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

Board::Board()
{
  setup();
}

Board::Board(int)
: m_white_can_short_castle(false),
  m_white_can_long_castle(false),
  m_black_can_short_castle(false),
  m_black_can_long_castle(false)
{
}

Bitboard Board::get_piece_set(Color color, Piece piece) const
{
  return m_bitboards[static_cast<uint8_t>(color)] & m_bitboards[static_cast<uint8_t>(piece)];
}

Bitboard Board::get_all(Piece piece) const
{
  return m_bitboards[static_cast<uint8_t>(piece)];
}

Bitboard Board::get_all(Color color) const
{
  return m_bitboards[static_cast<uint8_t>(color)];
}

Bitboard Board::get_en_passant_square() const
{
  return m_en_passant_square;
}

Bitboard Board::get_occupied_squares() const
{
  return m_bitboards[static_cast<uint8_t>(Color::white)] | m_bitboards[static_cast<uint8_t>(Color::black)];
}

bool Board::is_occupied(Coordinates square) const
{
  return get_occupied_squares().is_set(square);
}

Color Board::get_piece_color(Coordinates square) const
{
  MY_ASSERT(is_occupied(square), "This function can only be called for occupied squares");

  return get_all(Color::white).is_set(square) ? Color::white : Color::black;
}

Piece Board::get_piece(Coordinates square) const
{
  constexpr static std::array piece_types{Piece::pawn, Piece::bishop, Piece::knight,
                                          Piece::rook, Piece::queen, Piece::king};
  auto const search = std::find_if(piece_types.cbegin(), piece_types.cend(), [&](Piece piece)
                                   {
                                     return get_all(piece).is_set(square);
                                   });

  return (search == piece_types.cend()) ? Piece::empty : *search;
}

std::optional<std::pair<Coordinates, Piece>> Board::perform_move_(Move m, Coordinates capture_location)
{
  std::optional<std::pair<Coordinates, Piece>> captured_piece;
  auto const piece = get_piece(m.from);
  auto const color = get_piece_color(m.from);

  if (is_occupied(m.to) || capture_location != m.to)
  {
    captured_piece = std::pair{capture_location, get_piece(capture_location)};
    remove_piece_(opposite_color(color), captured_piece->second, capture_location);
  }

  add_piece_(color, piece, m.to);
  remove_piece_(color, piece, m.from);

  return captured_piece;
}

void Board::unperform_move_(Move m, std::optional<std::pair<Coordinates, Piece>> captured_piece)
{
  MY_ASSERT(is_occupied(m.to) && !is_occupied(m.from),
            "This function can only be called after a move has been performed");

  auto const piece = get_piece(m.to);
  auto const color = get_piece_color(m.to);

  remove_piece_(color, piece, m.to);
  add_piece_(color, piece, m.from);

  if (captured_piece)
  {
    add_piece_(opposite_color(color), captured_piece->second, captured_piece->first);
  }
}

bool Board::try_move(Move m)
{
  auto const piece_to_move = get_piece(m.from);
  if (piece_to_move == Piece::empty)
  {
    return false;
  }

  auto const color = get_piece_color(m.from);

  if (active_color() != color)
  {
    // Make sure the correct color is moving
    return false;
  }

  if (!piece_can_move(m.from, m.to, *this))
  {
    return false;
  }

  // Ensure that a promotion square is present iff we have a pawn move to the back rank
  if ((piece_to_move == Piece::pawn && (m.to.y() == 0 || m.to.y() == 7)) != (m.promotion != Piece::empty))
  {
    return false;
  }

  if (m.promotion == Piece::king || m.promotion == Piece::pawn)
  {
    // Cannot promote to king or pawn
    return false;
  }

  auto capture_location = m.to;
  if (piece_to_move == Piece::pawn && is_clear_diagonal(m.from, m.to) && !is_occupied(m.to))
  {
    // Update capture location for En passant case
    capture_location = previous_move()->to;
  }

  auto captured_piece = perform_move_(m, capture_location);
  if (is_in_check_(color))
  {
    unperform_move_(m, captured_piece);
    return false;
  }

  update_castling_rights_(color, piece_to_move, m.from);

  // Move rook if the move was a castle
  if (piece_to_move == Piece::king && distance_between(m.from, m.to) == 2)
  {
    auto rook_move = find_castling_rook_move_(m.to);
    perform_move_(rook_move, rook_move.to);
  }

  // Promote pawn if it reached the last rank
  if (m.promotion != Piece::empty)
  {
    remove_piece_(color, Piece::pawn, m.to);
    add_piece_(color, m.promotion, m.to);
  }

  // Set en passant square if applicable
  if (piece_to_move == Piece::pawn && distance_between(m.from, m.to) == 2)
  {
    int const offset = (color == Color::white) ? -1 : 1;
    m_en_passant_square.set_square(Coordinates{m.to.x(), m.to.y() + offset});
  }
  else
  {
    m_en_passant_square.unset_all();
  }

  m_previous_move = Move{m.from, m.to};

  MY_ASSERT(validate_(), "Board is in an incorrect state after move");
  return true;
}

bool Board::try_move_algebraic(std::string_view move_str)
{
  if (auto m = move_from_algebraic_(move_str, active_color()))
  {
    return try_move(*m);
  }
  return false;
}

bool Board::try_move_uci(std::string_view move_str)
{
  if (auto m = move_from_uci_(std::string{move_str}))
  {
    return try_move(*m);
  }
  return false;
}

Color Board::active_color() const
{
  if (!previous_move())
  {
    return Color::white;
  }

  return opposite_color(get_piece_color(previous_move()->to));
}

Move Board::find_castling_rook_move_(Coordinates king_destination) const
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

void Board::update_castling_rights_(Color color, Piece piece, Coordinates from)
{
  if (piece == Piece::king)
  {
    if (color == Color::black)
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

  if (piece == Piece::rook)
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
  // White pieces
  add_piece_(Color::white, Piece::rook, {0, 0});
  add_piece_(Color::white, Piece::knight, {1, 0});
  add_piece_(Color::white, Piece::bishop, {2, 0});
  add_piece_(Color::white, Piece::queen, {3, 0});
  add_piece_(Color::white, Piece::king, {4, 0});
  add_piece_(Color::white, Piece::bishop, {5, 0});
  add_piece_(Color::white, Piece::knight, {6, 0});
  add_piece_(Color::white, Piece::rook, {7, 0});
  for (int8_t i{0}; i < c_board_dimension; ++i)
  {
    add_piece_(Color::white, Piece::pawn, {i, 1});
  }

  // Black pieces
  add_piece_(Color::black, Piece::rook, {0, 7});
  add_piece_(Color::black, Piece::knight, {1, 7});
  add_piece_(Color::black, Piece::bishop, {2, 7});
  add_piece_(Color::black, Piece::queen, {3, 7});
  add_piece_(Color::black, Piece::king, {4, 7});
  add_piece_(Color::black, Piece::bishop, {5, 7});
  add_piece_(Color::black, Piece::knight, {6, 7});
  add_piece_(Color::black, Piece::rook, {7, 7});
  for (int8_t i{0}; i < c_board_dimension; ++i)
  {
    add_piece_(Color::black, Piece::pawn, {i, 6});
  }

  m_white_can_short_castle = true;
  m_white_can_long_castle = true;
  m_black_can_short_castle = true;
  m_black_can_long_castle = true;
  m_previous_move = {};

  MY_ASSERT(validate_(), "Board is in an incorrect state");
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
    if (is_occupied({from.x(), i}))
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
    if (is_occupied({i, from.y()}))
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
  int direction = 1;
  if (left.y() > right.y())
  {
    direction = -1;
  }

  // Walk from "left" to "right"
  for (int i = 1; i < right.x() - left.x(); i++)
  {
    // Check to see if square is occupied
    if (is_occupied({left.x() + i, left.y() + direction * i}))
    {
      return false;
    }
  }
  return true;
}

std::optional<Move> Board::previous_move() const
{
  return m_previous_move;
}

bool Board::is_in_check_(Color color) const
{
  Bitboard attacked_squares = Move_generator::get_all_attacked_squares(*this, opposite_color(color));
  Bitboard king_location = get_piece_set(color, Piece::king);
  return !(king_location & attacked_squares).is_empty();
}

bool Board::is_in_checkmate(Color color) const
{
  if (active_color() != color)
  {
    // A player cannot be in checkmate if it is not their turn
    return false;
  }

  if (!is_in_check_(color))
  {
    return false;
  }

  // We can cast this to non-const, since every change will be undone
  auto non_const_this = const_cast<Board*>(this);

  auto king_location = Coordinates{get_piece_set(color, Piece::king).bitscan_forward()};
  bool result = true;
  for (int i{-1}; i <= 1 && result; ++i)
  {
    for (int j{-1}; j <= 1 && result; ++j)
    {
      if (i != 0 || j != 0)
      {
        auto x = king_location.x() + i;
        auto y = king_location.y() + j;
        if (x > 0 && x < c_board_dimension && y > 0 && y < c_board_dimension)
        {
          Move m{king_location, {x, y}};
          if (is_occupied(m.to) && get_piece_color(m.to) != color)
          {
            auto captured_piece = non_const_this->perform_move_(m, m.to);
            result = is_in_check_(color);
            non_const_this->unperform_move_(m, captured_piece);
          }
        }
      }
    }
  }
  return result;
}

void Board::remove_piece_(Color color, Piece piece, Coordinates to_remove)
{
  MY_ASSERT(m_bitboards[static_cast<uint8_t>(color)].is_set(to_remove), "Cannot remove piece that is not present");
  MY_ASSERT(m_bitboards[static_cast<uint8_t>(piece)].is_set(to_remove), "Cannot remove piece that is not present");

  m_bitboards[static_cast<uint8_t>(color)].unset_square(to_remove);
  m_bitboards[static_cast<uint8_t>(piece)].unset_square(to_remove);
}

void Board::add_piece_(Color color, Piece piece, Coordinates to_add)
{
  MY_ASSERT(!m_bitboards[static_cast<uint8_t>(color)].is_set(to_add), "Cannot add piece that is already present");
  MY_ASSERT(!m_bitboards[static_cast<uint8_t>(piece)].is_set(to_add), "Cannot add piece that is already present");

  m_bitboards[static_cast<uint8_t>(color)].set_square(to_add);
  m_bitboards[static_cast<uint8_t>(piece)].set_square(to_add);
}

bool Board::validate_() const
{
  if ((get_piece_set(Color::black, Piece::pawn) | get_piece_set(Color::black, Piece::bishop) |
       get_piece_set(Color::black, Piece::knight) | get_piece_set(Color::black, Piece::rook) |
       get_piece_set(Color::black, Piece::queen) | get_piece_set(Color::black, Piece::king)) != get_all(Color::black))
  {
    return false;
  }

  if ((get_piece_set(Color::white, Piece::pawn) | get_piece_set(Color::white, Piece::bishop) |
       get_piece_set(Color::white, Piece::knight) | get_piece_set(Color::white, Piece::rook) |
       get_piece_set(Color::white, Piece::queen) | get_piece_set(Color::white, Piece::king)) != get_all(Color::white))
  {
    return false;
  }

  // Ensure that the same bit is not set for multiple different piece types
  constexpr static std::array piece_types{Piece::pawn, Piece::knight, Piece::bishop,
                                          Piece::rook, Piece::queen, Piece::king};
  for (auto piece1 : piece_types)
  {
    for (auto piece2 : piece_types)
    {
      if (piece1 != piece2)
      {
        if (!(get_all(piece1) & get_all(piece2)).is_empty())
        {
          return false;
        }
      }
    }
  }

  return true;
}

std::vector<Coordinates> Board::find_pieces_that_can_move_to(Piece piece, Color color, Coordinates target_square) const
{
  std::vector<Coordinates> candidates;
  auto locations = get_piece_set(color, piece);

  for (auto index : locations)
  {
    Coordinates location{index};
    if (piece_can_move(location, target_square, *this))
    {
      candidates.push_back(location);
    }
  }

  return candidates;
}

std::optional<Move> Board::move_from_uci_(std::string move_str) const
{
  move_str.erase(std::remove_if(move_str.begin(), move_str.end(), isspace), move_str.end());
  std::transform(move_str.begin(), move_str.end(), move_str.begin(), toupper);

  Piece promotion_result{Piece::empty};
  if (move_str.size() == 5 && !isdigit(move_str.back()))
  {
    promotion_result = from_char(toupper(move_str.back()));
    move_str.resize(move_str.size() - 1);
  }

  auto from = Coordinates::from_str(move_str);
  auto to = Coordinates::from_str({move_str.c_str() + 2, 2});

  if (from && to)
  {
    // TODO: Check for castling and en passant
    return Move{*from, *to, Move_type::normal, promotion_result};
  }

  return {};
}

std::optional<Move> Board::move_from_algebraic_(std::string_view move_param, Color color) const
{
  std::string move_str{move_param};
  move_str.erase(std::remove_if(move_str.begin(), move_str.end(),
                                [chars = std::string("x+#?!")](char c)
                                {
                                  return isspace(c) || (chars.find(c) != std::string::npos);
                                }),
                 move_str.end());

  Piece promotion_result{Piece::empty};
  if (auto index = move_str.find('='); index != std::string::npos)
  {
    if (index + 1 >= move_str.size())
    {
      std::cerr << "Invalid promotion target" << std::endl;
      return {};
    }
    promotion_result = from_char(toupper(move_str[index + 1]));
    move_str.resize(move_str.size() - 2);
  }

  if (move_str == "O-O" || move_str == "0-0")
  {
    if (color == Color::white)
    {
      return Move{Coordinates{4, 0}, Coordinates{6, 0}, Move_type::kingside_castle, promotion_result};
    }
    else
    {
      return Move{Coordinates{4, 7}, Coordinates{6, 7}, Move_type::kingside_castle, promotion_result};
    }
  }

  if (move_str == "O-O-O" || move_str == "0-0-0")
  {
    if (color == Color::white)
    {
      return Move{Coordinates{4, 0}, Coordinates{2, 0}, Move_type::queenside_castle, promotion_result};
    }
    else
    {
      return Move{Coordinates{4, 7}, Coordinates{2, 7}, Move_type::queenside_castle, promotion_result};
    }
  }

  // Read the square from the last two characters in the string
  auto target_square = Coordinates::from_str({move_str.c_str() + move_str.size() - 2, 2});
  move_str.resize(move_str.size() - 2); // Drop target square from string

  auto const piece = [&]
  {
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

  auto candidates = find_pieces_that_can_move_to(piece, color, *target_square);
  if (candidates.empty())
  {
    std::cerr << "No piece can perform move " << move_param << "\n";
    return {};
  }

  if (candidates.size() == 1)
  {
    // Exactly one piece can move to the target square

    // TODO: Check for en passent
    return Move{candidates.front(), *target_square, Move_type::normal, promotion_result};
  }

  if (move_str.empty())
  {
    std::cerr << "Too many pieces can perform move " << move_param << "\n";
    return {};
  }

  if (isalpha(move_str[0]))
  {
    // Drop candidates that are not on the correct column
    auto start_column = static_cast<int32_t>(move_str[0] - 'a');
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                    [start_column](Coordinates piece_loc)
                                    {
                                      return piece_loc.x() != start_column;
                                    }),
                     candidates.end());

    move_str = move_str.substr(1);
    if (candidates.size() == 1)
    {
      // Exactly one piece can move to the target square
      // TODO: Check for en passent
      return Move{candidates.front(), *target_square, Move_type::normal, promotion_result};
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
    auto start_row = static_cast<int32_t>(move_str[0] - '1');
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                    [start_row](Coordinates piece_loc)
                                    {
                                      return piece_loc.y() != start_row;
                                    }),
                     candidates.end());
    move_str = move_str.substr(1);
    if (candidates.size() == 1)
    {
      // TODO: Check for en passent

      // Exactly one piece can move to the target square
      return Move{candidates.front(), *target_square, Move_type::normal, promotion_result};
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

  // Play out the target moves on the board, and ensure they are valid
  std::optional<Board> result = Board{};
  for (auto const& move_str : moves)
  {
    if (!result->try_move_algebraic(move_str))
    {
      return {};
    }
  }

  return result;
}

bool Board::update_castling_rights_fen_(char c)
{
  switch (c)
  {
    case 'k':
      m_black_can_short_castle = true;
      break;
    case 'q':
      m_black_can_long_castle = true;
      break;
    case 'K':
      m_white_can_long_castle = true;
      break;
    case 'Q':
      m_white_can_long_castle = true;
      break;
    default:
      return false;
  }
  return true;
}

std::string Board::castling_rights_to_fen_() const
{
  std::stringstream ss;
  if (m_white_can_short_castle)
  {
    ss << 'K';
  }
  if (m_white_can_long_castle)
  {
    ss << 'Q';
  }
  if (m_black_can_short_castle)
  {
    ss << 'k';
  }
  if (m_black_can_long_castle)
  {
    ss << 'q';
  }

  auto result = ss.str();
  if (result.empty())
  {
    return "-";
  }
  return result;
}

std::optional<Board> Board::from_fen(std::string_view fen)
{
  std::optional<Board> board = Board{0};

  std::string fen_str{fen.substr(fen.find_first_not_of(" \n\t"))};

  int8_t y{c_board_dimension - 1};
  int8_t x{0};

  size_t index{0};
  while (index < fen_str.size() && (fen_str[index] != ' ') && y >= 0)
  {
    if (isdigit(fen_str[index]))
    {
      for (uint8_t i{0}; i < (fen_str[index] - '0'); ++i)
      {
        x = (x + 1) % c_board_dimension;
      }
    }
    else if (isalpha(fen_str[index]))
    {
      auto color = islower(fen_str[index]) ? Color::black : Color::white;
      auto piece = from_char(toupper(fen_str[index]));
      board->add_piece_(color, piece, {x, y});

      x = (x + 1) % c_board_dimension;
    }
    else if (fen_str[index] == '/')
    {
      if (x != 0)
      {
        std::cerr << "Badly formed fen string - not enough squares on row" << std::endl;
        return {};
      }
      --y;
    }

    ++index;
  }

  if (fen_str[index] != ' ')
  {
    std::cerr << "Badly formed fen string - Too many piece locations" << std::endl;
    return {};
  }
  ++index;

  Color to_play = Color::white;
  if (tolower(fen_str[index]) == 'w')
  {
    // White to play - denote this by setting previous_move to a black piece
    // The move being to and from the same square is irrelevent, the board will just check which color piece performed
    // the previous move
    Coordinates black_location{*board->get_all(Color::black).begin()};
    board->m_previous_move = Move{black_location, black_location};
  }
  else if (tolower(fen_str[index]) == 'b')
  {
    to_play = Color::black;

    // Black to play - denote this by setting previous_move to a white piece
    Coordinates white_location{*board->get_all(Color::white).begin()};
    board->m_previous_move = Move{white_location, white_location};
  }
  else
  {
    std::cerr << "Badly formed fen string - expected current move color" << std::endl;
    return {};
  }

  ++index;
  if (fen_str[index] != ' ')
  {
    std::cerr << "Badly formed fen string - expected space after current move" << std::endl;
    return {};
  }

  ++index;

  while (fen_str[index] != ' ')
  {
    board->update_castling_rights_fen_(fen_str[index]);
    ++index;
  }
  ++index;

  if (fen_str[index] != '-')
  {
    // En passant is possible, denote this by setting previous move to the pawn move that allowed en passant

    auto capture_square = Coordinates::from_str(std::string_view{fen_str.c_str() + index, 2});
    if (!capture_square)
    {
      std::cerr << "Badly formed fen string - invalid en passant capture square" << std::endl;
      return {};
    }

    board->m_en_passant_square.set_square(*capture_square);

    auto from_y = capture_square->y() + 1;
    auto to_y = capture_square->y() - 1;
    if (to_play == Color::black)
    {
      std::swap(from_y, to_y);
    }

    board->m_previous_move = Move{{capture_square->x(), from_y}, {capture_square->x(), to_y}};
  }

  MY_ASSERT(board->validate_(), "Invalid board created during fen parsing");

  return board;
}

std::string Board::to_fen() const
{
  std::stringstream result;

  auto to_char = [](Piece piece, Color color) -> char
  {
    std::stringstream ss;
    ss << piece;

    char result = ss.str().front();
    return (color == Color::white) ? toupper(result) : tolower(result);
  };

  for (int8_t y = c_board_dimension - 1; y >= 0; --y)
  {
    int empty_counter{0};
    for (int8_t x = 0; x < c_board_dimension; ++x)
    {
      Coordinates sq{x, y};
      if (is_occupied(sq))
      {
        if (empty_counter > 0)
        {
          result << std::to_string(empty_counter);
          empty_counter = 0;
        }
        result << to_char(get_piece(sq), get_piece_color(sq));
      }
      else
      {
        ++empty_counter;
      }
    }

    if (empty_counter > 0)
    {
      result << std::to_string(empty_counter);
      empty_counter = 0;
    }
    if (y > 0)
    {
      result << '/';
    }
  }
  result << ' ';
  result << ((active_color() == Color::white) ? 'w' : 'b');
  result << ' ';

  result << castling_rights_to_fen_();
  result << ' ';

  if (!get_en_passant_square().is_empty())
  {
    Coordinates en_passant_square{*get_en_passant_square().begin()};
    result << en_passant_square;
  }
  else
  {
    result << '-';
  }

  // We don't keep track of moves, so always write 0 and 1 for halfmove clock and fullmove clock
  result << " 0 1";

  return result.str();
}

std::string square_str(Coordinates location, Board const& board)
{
  auto const piece = board.get_piece(location);

  std::stringstream ss;
  ss << piece;

  ss << "_";

  if (piece != Piece::empty)
  {
    switch (board.get_piece_color(location))
    {
      case Color::black:
        ss << 'b';
        break;
      case Color::white:
        ss << 'w';
        break;
      default:
        MY_ASSERT(false, "Square occupier should be black or white");
        break;
    }
  }
  else
  {
    ss << "_";
  }

  ss << " ";
  return ss.str();
}

std::ostream& operator<<(std::ostream& out, Board const& self)
{
  out << std::endl;
  for (int i = c_board_dimension - 1; i >= 0; i--)
  {
    out << (i + 1) << "  ";
    for (int j = 0; j < c_board_dimension; j++)
    {
      out << square_str({j, i}, self);
    }
    out << std::endl
        << std::endl;
  }

  out << "   ";
  for (int i = 0; i < c_board_dimension; i++)
  {
    out << " " << static_cast<char>(i + 'A') << "  ";
  }
  out << std::endl;

  return out;
}
