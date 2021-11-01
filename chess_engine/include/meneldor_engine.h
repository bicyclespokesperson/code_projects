#ifndef MENELDOR_ENGINE
#define MENELDOR_ENGINE

#include "bitboard.h"
#include "chess_types.h"
#include "coordinates.h"

class Meneldor_engine
{
public:
  Meneldor_engine();

  void initialize_ray_attacks_();

  Bitboard gen_rook_moves_(Coordinates square, Bitboard occupied) const;

  Bitboard get_positive_ray_attacks_(Coordinates square, Compass_dir dir, Bitboard occupied) const;

private:
  // [square][direction]
  std::array<std::array<Bitboard, 8>, 64> m_ray_attacks{};
};

#endif // MENELDOR_ENGINE
