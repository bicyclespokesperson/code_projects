#ifndef MOVE_H
#define MOVE_H

#include "chess_types.h"
#include "coordinates.h"

struct Move
{
  Move(Coordinates from_coord, Coordinates to_coord, std::optional<Piece> promotion_piece = {})
      : from(from_coord), to(to_coord), promotion(promotion_piece)
  {
  }

  Coordinates from;
  Coordinates to;
  std::optional<Piece> promotion;
};

std::ostream& operator<<(std::ostream& os, Move const& self);

#endif // MOVE_H
