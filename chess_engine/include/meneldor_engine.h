#ifndef MENELDOR_ENGINE
#define MENELDOR_ENGINE

#include "bitboard.h"
#include "chess_types.h"

class Meneldor_engine
{
public:
  Meneldor_engine();

  void initialize_attacks();

private:
  std::array<std::array<Bitboard, 8>, 64> m_ray_attacks{};
};

#endif // MENELDOR_ENGINE
