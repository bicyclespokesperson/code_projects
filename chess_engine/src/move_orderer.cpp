#include "move_orderer.h"

void Move_orderer::sort_moves(std::span<Move> moves, Board const& /* board */) const
{
  // Sort by (victim value) - (attacker value)
  // This isn't quite MVV/LVA; it may be worth implementing that at some point.
  // This one penalizes any move that isn't a capture quite a bit, and prioritizes moving lower value pieces
  std::sort(moves.begin(), moves.end(),
            [](Move m1, Move m2)
            {
              // These correspond to the Piece enum type, so they can be indexed by static_cast<uint8>(Piece)
              constexpr static std::array piece_values{0, 0, 1, 3, 3, 5, 9, 10, 0};
              auto const m1_value =
                piece_values[static_cast<uint8_t>(m1.victim)] - piece_values[static_cast<uint8_t>(m1.piece)];
              auto const m2_value =
                piece_values[static_cast<uint8_t>(m2.victim)] - piece_values[static_cast<uint8_t>(m2.piece)];
              return m2_value < m1_value;
            });
}
