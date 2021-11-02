#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

static constexpr int32_t c_board_dimension{8};

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

  /*
  constexpr operator int32_t() const
  {
    return static_cast<int32_t>(m_value);
  }
  */

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

    return std::any_of(positive_directions.cbegin(), positive_directions.cend(), [&](auto dir) {
      return dir == this->m_value;
    });
  }

private:
  Value m_value;
};

constexpr Piece to_piece_enum(Color c);

Piece from_char(char c);

std::ostream& operator<<(std::ostream& os, Piece const& self);

#endif // CHESS_TYPES_H
