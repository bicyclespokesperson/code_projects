#include "move_generator.h"
#include "my_assert.h"

namespace
{

void update_if_in_bounds_(Bitboard& bb, int x, int y)
{
  if (0 <= x && x < c_board_dimension && 0 <= y && y < c_board_dimension)
  {
    bb.set_square(Coordinates{x, y}.square_index());
  }
}

} // namespace


Move_generator::Move_generator()
{
  initialize_ray_attacks_();
  initialize_knight_attacks_();
  initialize_king_attacks_();
}

void Move_generator::initialize_ray_attacks_()
{
  for (int8_t x{0}; x < c_board_dimension; ++x)
  {
    for (int8_t y{0}; y < c_board_dimension; ++y)
    {
      for (Compass_dir dir = Compass_dir::north; dir < Compass_dir::_count; ++dir)
      {
        auto square_index = Coordinates{x, y}.square_index();
        Bitboard& bb = m_ray_attacks[square_index][dir];

        int8_t square_x{x};
        int8_t square_y{y};
        bool first_iteration{true};

        // Set all squares in one direction to true.
        while (square_x >= 0 && square_x < c_board_dimension && square_y >= 0 && square_y < c_board_dimension)
        {
          // For a given square, we don't want the attacks based from that square to include the square itself
          if (!first_iteration)
          {
            bb.set_square({square_x, square_y});
          }
          first_iteration = false;

          switch (dir)
          {
          case Compass_dir::north:
            square_y += 1;
            break;
          case Compass_dir::northwest:
            square_y += 1;
            square_x -= 1;
            break;
          case Compass_dir::west:
            square_x -= 1;
            break;
          case Compass_dir::southwest:
            square_y -= 1;
            square_x -= 1;
            break;
          case Compass_dir::south:
            square_y -= 1;
            break;
          case Compass_dir::southeast:
            square_y -= 1;
            square_x += 1;
            break;
          case Compass_dir::east:
            square_x += 1;
            break;
          case Compass_dir::northeast:
            square_y += 1;
            square_x += 1;
            break;
          default:
            break;
          };
        } // while square in bounds
      }   // for each dir
    }     // for each y
  }       // for each x
} // initialize_ray_attacks

void Move_generator::initialize_knight_attacks_()
{
  for (int8_t x{0}; x < c_board_dimension; ++x)
  {
    for (int8_t y{0}; y < c_board_dimension; ++y)
    {
      auto square_index = Coordinates{x, y}.square_index();
      auto& bb = m_knight_attacks[square_index];

      update_if_in_bounds_(bb, x + 1, y + 2);
      update_if_in_bounds_(bb, x - 1, y + 2);
      update_if_in_bounds_(bb, x + 1, y - 2);
      update_if_in_bounds_(bb, x - 1, y - 2);
      update_if_in_bounds_(bb, x + 2, y + 1);
      update_if_in_bounds_(bb, x - 2, y + 1);
      update_if_in_bounds_(bb, x + 2, y - 1);
      update_if_in_bounds_(bb, x - 2, y - 1);
    }
  }
}

void Move_generator::initialize_king_attacks_()
{
  for (int8_t x{0}; x < c_board_dimension; ++x)
  {
    for (int8_t y{0}; y < c_board_dimension; ++y)
    {
      auto square_index = Coordinates{x, y}.square_index();
      auto& bb = m_king_attacks[square_index];

      for (int i = -1; i <= 1; ++i)
      {
        for (int j = -1; j <= 1; ++j)
        {
          if (i != 0 || j != 0)
          {
            update_if_in_bounds_(bb, x + i, y + j);
          }
        }
      }
    }
  }
}

Bitboard Move_generator::get_positive_ray_attacks(Coordinates square, Compass_dir dir, Bitboard occupied) const
{
  MY_ASSERT(dir.is_positive(), "Only positive directions are supported");

  Bitboard attacks = m_ray_attacks[square.square_index()][dir];
  Bitboard blockers = attacks & occupied;
  if (!blockers.is_empty())
  {
    auto const first_blocker = blockers.bitscan_forward();
    attacks ^= m_ray_attacks[first_blocker][dir];
  }

  return attacks;
}

Bitboard Move_generator::get_negative_ray_attacks(Coordinates square, Compass_dir dir, Bitboard occupied) const
{
  MY_ASSERT(!dir.is_positive(), "Only negative directions are supported");

  Bitboard attacks = m_ray_attacks[square.square_index()][dir];
  Bitboard blockers = attacks & occupied;
  if (!blockers.is_empty())
  {
    auto const first_blocker = blockers.bitscan_reverse();
    attacks ^= m_ray_attacks[first_blocker][dir];
  }

  return attacks;
}

Bitboard Move_generator::gen_sliding_moves_(std::span<const Compass_dir> positive_directions,
                                            std::span<const Compass_dir> negative_directions, Coordinates square,
                                            Bitboard occupied) const
{
  auto positive_attacks = std::accumulate(positive_directions.begin(), positive_directions.end(), Bitboard{0},
                                          [&](Bitboard result, Compass_dir dir) {
                                            return result | get_positive_ray_attacks(square, dir, occupied);
                                          });
  return std::accumulate(negative_directions.begin(), negative_directions.end(), positive_attacks,
                         [&](Bitboard result, Compass_dir dir) {
                           return result | get_negative_ray_attacks(square, dir, occupied);
                         });
}

Bitboard Move_generator::rook_attacks(Coordinates square, Bitboard occupied) const
{
  constexpr std::array<Compass_dir, 2> positive_directions{Compass_dir::north, Compass_dir::east};
  constexpr std::array<Compass_dir, 2> negative_directions{Compass_dir::south, Compass_dir::west};

  return gen_sliding_moves_(positive_directions, negative_directions, square, occupied);
}

Bitboard Move_generator::bishop_attacks(Coordinates square, Bitboard occupied) const
{
  constexpr std::array<Compass_dir, 2> positive_directions{Compass_dir::northeast, Compass_dir::northwest};
  constexpr std::array<Compass_dir, 2> negative_directions{Compass_dir::southeast, Compass_dir::southwest};

  return gen_sliding_moves_(positive_directions, negative_directions, square, occupied);
}

Bitboard Move_generator::queen_attacks(Coordinates square, Bitboard occupied) const
{
  return bishop_attacks(square, occupied) | rook_attacks(square, occupied);
}

Bitboard Move_generator::pawn_short_advances(Color color, Bitboard pawns, Bitboard occupied) const
{
  // TODO: Handle black moves

  auto one_space_moves = (pawns >> c_board_dimension) & ~occupied;
  return one_space_moves;
}

Bitboard Move_generator::pawn_long_advances(Color color, Bitboard pawns, Bitboard occupied) const
{
  // TODO: Handle black moves
  
  auto one_space_moves = ((pawns & bitboard_constants.second_rank) >> c_board_dimension) & ~occupied;
  auto two_space_moves = (one_space_moves >> c_board_dimension) & ~occupied;
  return two_space_moves;
}

Bitboard Move_generator::knight_attacks(Coordinates square, Bitboard /* occupied */) const
{
  return m_knight_attacks[square.square_index()];
}

Bitboard Move_generator::king_attacks(Coordinates square, Bitboard /* occupied */) const
{
  return m_king_attacks[square.square_index()];
}
