#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

enum class Color : uint8_t
{
  black = 0,
  white,
  num_colors,
};

enum class Piece : uint8_t
{
  black = static_cast<uint8_t>(Color::black), // Any black piece
  white = static_cast<uint8_t>(Color::white), // Any white piece
  pawn,
  knight,
  bishop,
  rook,
  queen,
  king,
  empty,
  num_pieces,
};

constexpr Piece to_piece_enum(Color c);

Piece from_char(char c);

std::ostream& operator<<(std::ostream& os, Piece const& self);


#endif // CHESS_TYPES_H
