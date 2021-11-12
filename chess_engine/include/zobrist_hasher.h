#ifndef ZOBRIST_HASHER_H
#define ZOBRIST_HASHER_H

#include "bitboard.h"
#include "move.h"

class Board;

using zhash_t = uint64_t;

class Zobrist_hasher
{
public:
  Zobrist_hasher();
  
  [[nodiscard]] zhash_t hash_from_position(Board const& board) const;
  [[nodiscard]] zhash_t update_en_passant_square(Bitboard ep_square, zhash_t zhash) const;
  [[nodiscard]] zhash_t update_castling_rights(Castling_rights rights, zhash_t zhash) const;
  [[nodiscard]] zhash_t update_with_move(Piece piece, Move m, zhash_t zhash) const;
  [[nodiscard]] zhash_t update_with_undo_move(Piece piece, Move m, zhash_t zhash) const;

private:
  [[nodiscard]] zhash_t update_player_to_move_(zhash_t zhash) const;
  std::array <zhash_t, 781> m_random_numbers{};
};

#endif // ZOBRIST_HASHER_H
