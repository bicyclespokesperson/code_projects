#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "bitboard.h"
#include "chess_types.h"
#include "coordinates.h"
#include "move.h"
#include "position.h"

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

  Bitboard pawn_east_attacks(Color color, Bitboard pawns, Bitboard enemies) const;
  Bitboard pawn_west_attacks(Color color, Bitboard pawns, Bitboard enemies) const;

  std::vector<Move> generate_piece_moves(Position const& position, Color color) const;
  std::vector<Move> generate_pawn_moves(Position const& position, Color color) const;

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

struct Bitboard_constants
{
  static constexpr Bitboard first_rank{0x00000000000000ff};
  static constexpr Bitboard second_rank{0x000000000000ff00};
  static constexpr Bitboard third_rank{0x0000000000ff0000};
  static constexpr Bitboard forth_rank{0x00000000ff000000};
  static constexpr Bitboard fifth_rank{0x000000ff00000000};
  static constexpr Bitboard sixth_rank{0x0000ff0000000000};
  static constexpr Bitboard seventh_rank{0x00ff000000000000};
  static constexpr Bitboard eighth_rank{0xff00000000000000};
  static constexpr Bitboard a_file{0x0101010101010101};
  static constexpr Bitboard h_file{0x8080808080808080};
  static constexpr Bitboard all{std::numeric_limits<uint64_t>::max()};
  static constexpr Bitboard none{0};
};

#endif // MOVE_GENERATOR_H
