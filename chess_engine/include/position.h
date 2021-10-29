#ifndef POSITION_H
#define POSITION_H

#include "bitboard.h"
#include "chess_types.h"

class Position
{
public:
  Bitboard get_piece_set(Piece piece, Color color) const
  {
    return m_bitboards[static_cast<size_t>(piece)].val & m_bitboards[static_cast<size_t>(color)].val;
  }

  Bitboard get_all(Piece piece)
  {
    return m_bitboards[static_cast<size_t>(piece)];
  }

private:
  // Store one bitboard for each piece type (e.g. all pawns, regardless of color)
  // Also store one for all white pieces and one for all black pieces
  std::array<Bitboard, static_cast<size_t>(Piece::num_pieces)> m_bitboards;
};

#endif // POSITION_H
