#include "chess_types.h"

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
      break;
    default:
      os << '-';
      break;
  }

  return os;
}
