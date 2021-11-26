#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "bitboard.h"
#include "chess_types.h"
#include "coordinates.h"
#include "move.h"

class Board;

class Move_generator
{
public:
  static uint64_t perft(int depth, Board& board, std::atomic_flag& is_cancelled);

  static std::vector<Move> generate_legal_moves(Board const& board);
  static std::vector<Move> generate_legal_attack_moves(Board const& board);
  static std::vector<Move> generate_pseudo_legal_moves(Board const& board);
  static bool has_any_legal_moves(Board const& board);

  static void generate_piece_moves(Board const& board, Color color, std::vector<Move>& moves);
  static void generate_piece_attacks(Board const& board, Color color, std::vector<Move>& moves);
  static void generate_pawn_moves(Board const& board, Color color, std::vector<Move>& moves);
  static void generate_pawn_attacks(Board const& board, Color color, std::vector<Move>& moves);
  static void generate_castling_moves(Board const& board, Color color, std::vector<Move>& moves);
  static Bitboard get_all_attacked_squares(Board const& board, Color attacking_color);

  static Bitboard rook_attacks(Coordinates square, Bitboard occupied);
  static Bitboard bishop_attacks(Coordinates square, Bitboard occupied);

  static Bitboard queen_attacks(Coordinates square, Bitboard occupied);
  static Bitboard knight_attacks(Coordinates square, Bitboard occupied);
  static Bitboard king_attacks(Coordinates square, Bitboard occupied);

  static Bitboard pawn_short_advances(Color color, Bitboard pawns, Bitboard occupied);
  static Bitboard pawn_long_advances(Color color, Bitboard pawns, Bitboard occupied);
  static Bitboard pawn_potential_attacks(Color color, Bitboard pawns);

  static Bitboard pawn_east_attacks(Color color, Bitboard pawns, Bitboard enemies);
  static Bitboard pawn_west_attacks(Color color, Bitboard pawns, Bitboard enemies);

private:
  class Tables
  {
  public:
    struct Magic
    {
      Bitboard mask{0}; // to mask relevant squares of both lines (no outer squares)
      uint64_t magic{0}; // magic 64-bit factor
    };

    Tables();

    // [square][direction]
    std::array<std::array<Bitboard, Compass_dir::_count>, c_board_dimension_squared> ray_attacks;

    std::array<Bitboard, c_board_dimension_squared> knight_attacks;

    std::array<Bitboard, c_board_dimension_squared> king_attacks;

    std::array<Magic, 64> bishop_magic_table;
    std::array<Magic, 64> rook_magic_table;

    std::array<std::array<Bitboard, 512>, 64> bishop_attacks{};
    std::array<std::array<Bitboard, 4096>, 64> rook_attacks{};

    void init_bishop_magic_tables_(int index);
    void init_rook_magic_tables_(int index);

  private:
    void initialize_knight_attacks_();
    void initialize_king_attacks_();
  };

  static Bitboard get_positive_ray_attacks_(Coordinates square, Compass_dir dir, Bitboard occupied);
  static Bitboard get_negative_ray_attacks_(Coordinates square, Compass_dir dir, Bitboard occupied);

  static Bitboard gen_sliding_moves_(std::span<const Compass_dir> positive_directions,
                                     std::span<const Compass_dir> negative_directions,
                                     Coordinates square,
                                     Bitboard occupied);

  static const Tables m_tables;
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
  static constexpr Bitboard all_outer_squares{0xff818181818181ff};
  static constexpr Bitboard corners{0x8100000000000081};

  static constexpr Bitboard short_castling_empty_squares_white{0x0000000000000060};
  static constexpr Bitboard long_castling_empty_squares_white{0x000000000000000c};
  static constexpr Bitboard short_castling_empty_squares_black{0x6000000000000000};
  static constexpr Bitboard long_castling_empty_squares_black{0x0c00000000000000};

  static constexpr Bitboard all{std::numeric_limits<uint64_t>::max()};
  static constexpr Bitboard none{0};
};

#endif // MOVE_GENERATOR_H
