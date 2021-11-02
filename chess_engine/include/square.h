#ifndef SQUARE_H
#define SQUARE_H

#include "chess_types.h"

class Square
{
public:
  Square() : m_occupier(Piece::empty), m_color(Color::black)
  {
  }

  Square(Piece piece, Color piece_color) : m_occupier(piece), m_color(piece_color)
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

  bool is_white() const;

private:
  Piece m_occupier : 5;
  Color m_color : 1;
};

std::ostream& operator<<(std::ostream& os, Square const& self);

#endif
