#ifndef ZOBRIST_HASH_H
#define ZOBRIST_HASH_H

#include "bitboard.h"
#include "move.h"

class Board;

using zhash_t = uint64_t;

class Zobrist_hash
{
public:
  Zobrist_hash() = default;
  Zobrist_hash(Board const& board);
  Zobrist_hash(Zobrist_hash const& other) = default;
  Zobrist_hash(Zobrist_hash&& other) = default;
  Zobrist_hash& operator=(Zobrist_hash const& other) = default;
  Zobrist_hash& operator=(Zobrist_hash&& other) = default;

  auto operator<=>(Zobrist_hash const& other) const = default;

  // These functions are their own inverses. To keep the hash up to date, they should be called
  // with both the old and new values of a change in board state, to unset the old value and then
  // set the new value.
  void update_en_passant_square(Bitboard ep_square);
  void update_castling_rights(Castling_rights rights);
  void update_piece_location(Color color, Piece piece, Coordinates location);

  // Should be called once when the player to move changes
  void update_player_to_move();

  zhash_t get_hash() const;

  constexpr operator zhash_t() const
  {
    return m_hash;
  }

private:
  zhash_t m_hash{0};

  static const std::array<zhash_t, 793> m_random_numbers;
};

std::ostream& operator<<(std::ostream& os, Zobrist_hash const& self);

#endif // ZOBRIST_HASH_H
