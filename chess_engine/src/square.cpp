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

void Square::display(std::ostream& out) const
{
  std::string result{"___ "};

  switch (occupier())
  {
  case Piece::pawn:
    result[0] = 'P';
    break;
  case Piece::bishop:
    result[0] = 'B';
    break;
  case Piece::knight:
    result[0] = 'N';
    break;
  case Piece::rook:
    result[0] = 'R';
    break;
  case Piece::queen:
    result[0] = 'Q';
    break;
  case Piece::king:
    result[0] = 'K';
    break;
  case Piece::empty:
    [[fallthrough]];
  default:
    break;
  }

  if (is_occupied())
  {
    switch (occupier_color())
    {
    case Color::black:
      result[2] = 'b';
      break;
    case Color::white:
      result[2] = 'w';
      break;
    }
  }

  out << result;
}

bool Square::is_white() const
{
  MY_ASSERT(is_occupied(), "Square color is irrelevant for an empty square");
  return occupier_color() == Color::white;
}
