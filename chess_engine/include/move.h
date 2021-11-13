#ifndef MOVE_H
#define MOVE_H

#include "chess_types.h"
#include "coordinates.h"

struct Move
{
  constexpr Move(Coordinates from_coord,
                 Coordinates to_coord,
                 Piece moving_piece,
                 Piece victim_piece,
                 Piece promotion_piece = Piece::empty,
                 Move_type move_type = Move_type::normal)
  : from(from_coord),
    to(to_coord),
    piece(moving_piece),
    victim(victim_piece),
    promotion(promotion_piece),
    type(move_type)
  {
    MY_ASSERT(!(static_cast<uint8_t>(moving_piece) & 0xf0), "Ensure we aren't throwing away any information about the piece");
    MY_ASSERT(!(static_cast<uint8_t>(victim_piece) & 0xf0), "Ensure we aren't throwing away any information about the piece");
    MY_ASSERT(!(static_cast<uint8_t>(promotion_piece) & 0xf0), "Ensure we aren't throwing away any information about the piece");
  }

  Coordinates from; // 8
  Coordinates to;   // 8
  Piece piece : 4;

  // Piece::empty represents no piece will be captured by this move
  Piece victim : 4;
  Piece promotion : 4;
  Move_type type : 4;
};

constexpr void test_fn()
{
  static_assert(sizeof(Move) == 4);
}

std::ostream& operator<<(std::ostream& os, Move const& self);

#endif // MOVE_H
