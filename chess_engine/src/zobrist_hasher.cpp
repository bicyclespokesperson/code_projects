#include "zobrist_hasher.h"
#include "board.h"

// https://www.chessprogramming.org/Zobrist_Hashing

namespace {
  
} // namespace

Zobrist_hasher::Zobrist_hasher()
{
  // Initialize with constant seed for run to run repeatability
  std::seed_seq seq{267403289, -1616180330, -870258931, -648938664, -1040157715, -195351127, -728458912, 1033321512};
  std::mt19937 eng(seq);
  std::uniform_int_distribution<int> dist{};

  std::generate(m_random_numbers.begin(), m_random_numbers.end(), [&]
                {
                  return dist(eng);
                });

}

zhash_t Zobrist_hasher::hash_from_position(Board const& board)
{
  //TODO: Implement
#if 0
  auto piece_offset = static_cast<uint8_t>(piece) - static_cast<uint8_t>(Piece::pawn);
  auto piece_count = static_cast<uint8_t>(Piece::_count) - static_cast<uint8_t>(Piece::pawn);
  auto index = piece_count * coords.square_index();
#endif

}

zhash_t Zobrist_hasher::update_with_move(Move m, zhash_t)
{
}

zhash_t Zobrist_hasher::update_with_undo_move(Move m, zhash_t)
{
}
