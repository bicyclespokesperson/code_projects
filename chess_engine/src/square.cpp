#include "square.h"

void Square::display(std::ostream& out) const
{
  std::string result{"___ "};

  switch (occupier())
  {
    case Piece::pawn:
      result[0] = 'P';
      break;
    case Piece::bishop_dark:
      [[fallthrough]];
    case Piece::bishop_light:
      result[0] = 'B';
      break;
    case Piece::knight:
      result[0] = 'N';
      break;
    case Piece::rook:
      result[0] = 'R';
      break;
    case Piece::queen:
      result[0] = '!';
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

