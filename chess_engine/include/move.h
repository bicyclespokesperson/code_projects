#ifndef MOVE_H
#define MOVE_H

#include "chess_types.h"
#include "coordinates.h"

struct Move
{
  Move(Coordinates from_coord, Coordinates to_coord, Move_type move_type = Move_type::normal, Piece promotion_piece = Piece::empty)
      : from(from_coord), to(to_coord), type(move_type), promotion(promotion_piece)
  {
  }

  Coordinates from;
  Coordinates to;
  Move_type type;
  Piece promotion;
};

std::ostream& operator<<(std::ostream& os, Move const& self);

#endif // MOVE_H
