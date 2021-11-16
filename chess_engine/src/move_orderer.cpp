#include "move_orderer.h"

const auto Move_orderer::mvv_lva_table = []
{
  static constexpr std::array pieces{Piece::king,   Piece::queen, Piece::rook, Piece::bishop,
                                     Piece::knight, Piece::pawn,  Piece::empty};

  //NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init) Initialized in this function
  std::array<std::array<int, c_piece_count>, c_piece_count> result;

  // Create a lookup table of move priorities. Move with the most valuable victim piece will be prioritied highest,
  // and moves with the same victim will be prioritized by the least valuable attacker. Higher priorities correspond
  // to better moves

  int current_val{1};

  // Start with the least valuable victim, and increment current_val as we go so
  // more valuable victims have higher priorities
  for (size_t victim{c_piece_count}; victim > 0; --victim)
  {
    for (size_t attacker{0}; attacker < c_piece_count; ++attacker)
    {
      auto victim_index = static_cast<uint8_t>(pieces[victim - 1]) - static_cast<uint8_t>(Piece::pawn);
      auto attacker_index = static_cast<uint8_t>(pieces[attacker]) - static_cast<uint8_t>(Piece::pawn);
      result[victim_index][attacker_index] = current_val;
      ++current_val;
    }
  }

  return result;
}();

void Move_orderer::sort_moves(std::span<Move> moves, Board const& /* board */) const
{
  std::sort(moves.begin(), moves.end(),
            [](Move m1, Move m2)
            {
              auto const m1_victim_index{static_cast<uint8_t>(m1.victim) - static_cast<uint8_t>(Piece::pawn)};
              auto const m1_attacker_index{static_cast<uint8_t>(m1.piece) - static_cast<uint8_t>(Piece::pawn)};
              auto const m2_victim_index{static_cast<uint8_t>(m2.victim) - static_cast<uint8_t>(Piece::pawn)};
              auto const m2_attacker_index{static_cast<uint8_t>(m2.piece) - static_cast<uint8_t>(Piece::pawn)};

              return mvv_lva_table[m1_victim_index][m1_attacker_index] >
                     mvv_lva_table[m2_victim_index][m2_attacker_index];
            });
}
