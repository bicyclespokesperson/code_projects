#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "bitboard.h"
#include "chess_types.h"
#include "coordinates.h"

class Move_generator
{
public:
  Move_generator();

  Bitboard gen_rook_moves(Coordinates square, Bitboard occupied) const;
  Bitboard gen_bishop_moves(Coordinates square, Bitboard occupied) const;
  Bitboard gen_queen_moves(Coordinates square, Bitboard occupied) const;
  Bitboard gen_pawn_moves(Coordinates square, Bitboard occupied) const;
  Bitboard gen_knight_moves(Coordinates square, Bitboard occupied) const;
  Bitboard gen_king_moves(Coordinates square, Bitboard occupied) const;

  Bitboard get_positive_ray_attacks(Coordinates square, Compass_dir dir, Bitboard occupied) const;
  Bitboard get_negative_ray_attacks(Coordinates square, Compass_dir dir, Bitboard occupied) const;

private:
  void initialize_ray_attacks_();
  void initialize_knight_attacks_();
  void initialize_king_attacks_();

  Bitboard gen_sliding_moves_(std::span<const Compass_dir> positive_directions,
                              std::span<const Compass_dir> negative_directions, Coordinates square,
                              Bitboard occupied) const;

  // [square][direction]
  std::array<std::array<Bitboard, Compass_dir::_count>, c_board_dimension_squared> m_ray_attacks{};
  std::array<Bitboard, c_board_dimension_squared> m_knight_attacks{};
  std::array<Bitboard, c_board_dimension_squared> m_king_attacks{};
};

#endif // MOVE_GENERATOR_H
