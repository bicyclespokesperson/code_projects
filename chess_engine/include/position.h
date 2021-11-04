#ifndef POSITION_H
#define POSITION_H

#include "bitboard.h"
#include "chess_types.h"

// TODO: Integrate this with the Board class. May mean keeping a position object on the board, or maybe just adding these methods directly to the board
class Position
{
public:
  Bitboard get_piece_set(Color color, Piece piece) const
  {
    return m_bitboards[static_cast<int32_t>(piece)] & m_bitboards[static_cast<int32_t>(color)];
  }

  Bitboard get_all(Piece piece) const
  {
    return m_bitboards[static_cast<int32_t>(piece)];
  }

  Bitboard get_all(Color color) const
  {
    return m_bitboards[static_cast<int32_t>(color)];
  }

  Bitboard get_occupied() const
  {
    return m_bitboards[static_cast<int32_t>(Color::white)] | m_bitboards[static_cast<int32_t>(Color::black)];
  }

private:
  // Store one bitboard for each piece type (e.g. all pawns, regardless of color)
  // Also store one for all white pieces and one for all black pieces
  std::array<Bitboard, static_cast<size_t>(Piece::_count)> m_bitboards;
};

#endif // POSITION_H
