#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

static constexpr int32_t c_board_dimension{8};
static constexpr int32_t c_board_dimension_squared{c_board_dimension * c_board_dimension};

enum class Color : uint8_t
{
  black = 0,
  white,
  _count,
};

enum class Piece : uint8_t
{
  black = static_cast<uint8_t>(Color::black), // Any black piece
  white = static_cast<uint8_t>(Color::white), // Any white piece
  pawn,
  knight,
  bishop,
  rook,
  queen,
  king,
  empty,
  _count,
};

enum class Move_type : uint8_t
{
  normal = 0,
  en_passant,
  kingside_castle, //TODO: Are these useful for anything? // Could I pack en passant in somewhere else if only two flags are needed?
  queenside_castle,
  _count,
};

struct Castling_rights
{
  bool white_can_short_castle : 1 {true};
  bool white_can_long_castle : 1 {true};
  bool black_can_short_castle : 1 {true};
  bool black_can_long_castle : 1 {true};
};

class Compass_dir
{
public:
  enum Value : int32_t
  {
    north,
    northwest,
    west,
    southwest,
    south,
    southeast,
    east,
    northeast,
    _count,
  };

  constexpr Compass_dir(Value dir) : m_value(dir)
  {
  }

  constexpr operator Value() const
  {
    return m_value;
  }

  constexpr Compass_dir& operator++()
  {
    m_value = static_cast<Value>(static_cast<int32_t>(m_value) + 1);
    return *this;
  }

  explicit operator bool() = delete;

  // These directions count as positive on this board representation
  constexpr bool is_positive() const
  {
    constexpr std::array positive_directions{Compass_dir::north, Compass_dir::east, Compass_dir::northwest,
                                             Compass_dir::northeast};

    return std::any_of(positive_directions.cbegin(), positive_directions.cend(),
                       [&](auto dir)
                       {
                         return dir == this->m_value;
                       });
  }

private:
  Value m_value;
};

class Threefold_repetition_detector
{
public:
  Threefold_repetition_detector() = default;

  // Call this after every move, passing it a fen str of the current game
  // returns true if the game is a draw due to threefold repetition
  bool add_fen(std::string_view fen);

  bool is_drawn() const;

private:
  std::unordered_map<std::string, uint8_t> m_previous_positions;
  bool m_is_drawn{false};
};

constexpr Piece to_piece_enum(Color c)
{
  return static_cast<Piece>(c);
}

constexpr Color opposite_color(Color color)
{
  return static_cast<Color>(1 - static_cast<uint8_t>(color));
}

Piece from_char(char c);

std::ostream& operator<<(std::ostream& os, Piece const& self);

std::ostream& operator<<(std::ostream& os, Color const& self);

#endif // CHESS_TYPES_H
