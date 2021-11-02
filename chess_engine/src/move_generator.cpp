#include "move_generator.h"
#include "my_assert.h"

Move_generator::Move_generator()
{
  initialize_ray_attacks_();
}

void Move_generator::initialize_ray_attacks_()
{
  for (int8_t x{0}; x < c_board_dimension; ++x)
  {
    for (int8_t y{0}; y < c_board_dimension; ++y)
    {
      for (Compass_dir dir = Compass_dir::north; dir < Compass_dir::_count; ++dir)
      {
        // https://www.chessprogramming.org/Classical_Approach

        int square_index = y * c_board_dimension + x;
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
      } // for each dir
    } // for each y
  } // for each x
} // initialize_ray_attacks

Bitboard Move_generator::get_positive_ray_attacks(Coordinates square, Compass_dir dir, Bitboard occupied) const
{
  constexpr std::array valid_directions{Compass_dir::north, Compass_dir::east, Compass_dir::northwest, Compass_dir::northeast};
  MY_ASSERT(std::find(valid_directions.cbegin(), valid_directions.cend(), dir) != valid_directions.cend(), "Only positive directions are supported");

  Bitboard attacks = m_ray_attacks[square.square_index()][dir];
  Bitboard blockers = attacks & occupied;
  if (!blockers.is_empty())
  {
    auto first_blocker = blockers.bitscan_forward();
    attacks ^= m_ray_attacks[first_blocker][dir];
  }

  return attacks;
}

Bitboard Move_generator::get_negative_ray_attacks(Coordinates square, Compass_dir dir, Bitboard occupied) const
{
  constexpr std::array valid_directions{Compass_dir::south, Compass_dir::west, Compass_dir::southwest, Compass_dir::southeast};
  MY_ASSERT(std::find(valid_directions.cbegin(), valid_directions.cend(), dir) != valid_directions.cend(), "Only positive directions are supported");

  Bitboard attacks = m_ray_attacks[square.square_index()][dir];
  Bitboard blockers = attacks & occupied;
  if (!blockers.is_empty())
  {
    auto first_blocker = blockers.bitscan_forward();
    attacks ^= m_ray_attacks[first_blocker][dir];
  }

  return attacks;
}

Bitboard Move_generator::gen_rook_moves(Coordinates square, Bitboard occupied) const
{
  constexpr std::array positive_directions{Compass_dir::north, Compass_dir::east};
  constexpr std::array negative_directions{Compass_dir::south, Compass_dir::west};

  auto positive_attacks = std::accumulate(positive_directions.cbegin(), positive_directions.cend(), Bitboard{0}, [&](Bitboard result, Compass_dir dir)
                                          {
                                            return result | get_positive_ray_attacks(square, dir, occupied);
                                          });
  return  std::accumulate(negative_directions.cbegin(), negative_directions.cend(), positive_attacks, [&](Bitboard result, Compass_dir dir)
                          {
                            return result | get_negative_ray_attacks(square, dir, occupied);
                          });
}

