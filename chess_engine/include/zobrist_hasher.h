#ifndef ZOBRIST_HASHER_H
#define ZOBRIST_HASHER_H

#include "move.h"

class Board;

using zhash_t = uint64_t;

class Zobrist_hasher
{
public:
  Zobrist_hasher();
  
  zhash_t hash_from_position(Board const& board);
  zhash_t update_with_move(Move m, zhash_t);
  zhash_t update_with_undo_move(Move m, zhash_t);

private:
  std::array <zhash_t, 781> m_random_numbers{};
};

#endif // ZOBRIST_HASHER_H
