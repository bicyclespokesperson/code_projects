#include "meneldor_engine.h"

static constexpr int32_t c_board_dimension{8};

Meneldor_engine::Meneldor_engine()
{
  initialize_attacks();

  std::cout << "Printing out all attacks for b1 square" << std::endl;
  for (Compass_dir dir = Compass_dir::north; dir < Compass_dir::_count; ++dir)
  {
    std::cout << m_ray_attacks[28][dir] << std::endl;
  }
}

void Meneldor_engine::initialize_attacks()
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
}
