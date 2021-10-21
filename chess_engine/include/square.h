#ifndef SQUARE_H
#define SQUARE_H

enum class Piece : uint8_t
{
  empty = 0,
  pawn,
  knight,
  bishop_dark,
  bishop_light,
  rook,
  queen,
  king,
};

enum class Color : uint8_t
{
  black = 0,
  white,
};

enum class Square_color : uint8_t
{
  dark = 0,
  light,
};

class Square
{
public:
  Square() = default;

  Square(Piece piece, Color piece_color, Square_color sq_color)
  : m_occupier(piece),
    m_color(piece_color),
    m_square_color(sq_color)
  {
    static_assert(sizeof(Square) == 1);
  }

  bool is_occupied() const
  {
    return m_occupier != Piece::empty;
  }

  Piece occupier() const
  {
    return m_occupier;
  }

  void set_occupier(Piece new_occupier)
  {
    m_occupier = new_occupier;
  }

  void remove_occupier()
  {
    set_occupier(Piece::empty);
  }

  Color occupier_color() const
  {
    return m_color;
  }

  void set_occupier_color(Color new_color)
  {
    m_color = new_color;
  }

  Square_color square_color() const
  {
    return m_square_color;
  }

  void set_square_color(Square_color new_color)
  {
    m_square_color = new_color;
  }

  bool is_white() const;

  void display(std::ostream& out) const;

private:
  Piece m_occupier : 5;
  Color m_color : 1;
  Square_color m_square_color : 1;
};

#endif
