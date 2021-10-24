#include "square.h"
#include "my_assert.h"

Piece from_char(char c)
{
  switch (c)
  {
  case 'N':
    return Piece::knight;
  case 'B':
    return Piece::bishop;
  case 'R':
    return Piece::rook;
  case 'Q':
    return Piece::queen;
  case 'K':
    return Piece::king;
  case 'P':
    return Piece::pawn;
  default:
    return Piece::empty;
  }
}

std::ostream& operator<<(std::ostream& os, Piece const& self)
{
  switch (self)
  {
  case Piece::pawn:
    os << 'P';
    break;
  case Piece::bishop:
    os << 'B';
    break;
  case Piece::knight:
    os << 'N';
    break;
  case Piece::rook:
    os << 'R';
    break;
  case Piece::queen:
    os << 'Q';
    break;
  case Piece::king:
    os << 'K';
    break;
  case Piece::empty:
    os << '_';
  default:
    break;
  }

  return os;
}

std::ostream& operator<<(std::ostream& os, Square const& self)
{
  os << self.occupier();

  os << "_";

  if (self.is_occupied())
  {
    switch (self.occupier_color())
    {
    case Color::black:
      os << 'b';
      break;
    case Color::white:
      os << 'w';
      break;
    }
  }
  else
  {
    os << "_";
  }

  os << " ";
  return os;
}

bool Square::is_white() const
{
  MY_ASSERT(is_occupied(), "Square color is irrelevant for an empty square");
  return occupier_color() == Color::white;
}
