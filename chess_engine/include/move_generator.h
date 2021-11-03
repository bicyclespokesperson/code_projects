#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "bitboard.h"
#include "chess_types.h"
#include "coordinates.h"
#include "move.h"

struct Bitboard_constants
{
  constexpr Bitboard_constants()
  : all(std::numeric_limits<uint64_t>::max()),
    none(0)
{
  for (int i{0}; i < c_board_dimension; ++i)
  {
    second_rank.set_square({i, 6});
    third_rank.set_square({i, 2});
    seventh_rank.set_square({i, 1});
  }
}

  Bitboard second_rank;
  Bitboard third_rank;
  Bitboard seventh_rank;
  Bitboard all;
  Bitboard none;
};

class Move_generator
{
public:
  Move_generator();

  Bitboard rook_attacks(Coordinates square, Bitboard occupied) const;
  Bitboard bishop_attacks(Coordinates square, Bitboard occupied) const;
  Bitboard queen_attacks(Coordinates square, Bitboard occupied) const;
  Bitboard knight_attacks(Coordinates square, Bitboard occupied) const;
  Bitboard king_attacks(Coordinates square, Bitboard occupied) const;

  Bitboard pawn_short_advances(Color color, Bitboard pawns, Bitboard occupied) const;
  Bitboard pawn_long_advances(Color color, Bitboard pawns, Bitboard occupied) const;
  Bitboard pawn_promotions(Color color, Bitboard pawns, Bitboard occupied) const;
  Bitboard pawn_potential_attacks(Color color, Bitboard pawns) const;

  std::vector<Move> generate_piece_moves() const;
  std::vector<Move> generate_pawn_moves() const;

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
  Bitboard_constants bitboard_constants{};
};

#endif // MOVE_GENERATOR_H
