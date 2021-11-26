#include "move_generator.h"
#include "board.h"
#include "feature_toggle.h"
#include "my_assert.h"

namespace
{

constexpr bool is_debug(false);
constexpr int debug_square_index{3};
constexpr Bitboard debug_occupied_squares{0xfffe01000009f6ff};

std::array<int, 64> RBits{12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10,
                          10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10,
                          10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12};

std::array<int, 64> BBits{6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7,
                          5, 5, 5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7,
                          7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6};

// Generated with code from: https://www.chessprogramming.org/Looking_for_Magics
const std::array<uint64_t, 64> c_rook_magics{
  0xa8002c000108020ULL,  0x6040004109200130ULL, 0x5820000814a02040ULL, 0x400800122040c4ULL,   0xa00211004010204ULL,
  0x24010204000080ULL,   0x120004000822600ULL,  0x100024284a20700ULL,  0x4818080004040ULL,    0x4008e4804100ULL,
  0x20200018000400ULL,   0x22001020081a040ULL,  0x30400200102205ULL,   0x201280018c020201ULL, 0x10401000486040e0ULL,
  0x80290000804001ULL,   0x2000241002a08410ULL, 0x8018012008a84000ULL, 0x2015081004600090ULL, 0x8006803002208200ULL,
  0xc220101090c800ULL,   0x20800404018aa200ULL, 0x4800900102200ULL,    0x600400980002015ULL,  0x198688480024008ULL,
  0x10201040002000ULL,   0x403420002004c021ULL, 0x411081801804004ULL,  0x61004108016600ULL,   0x12400088240004ULL,
  0x4004280806000100ULL, 0x840a0220005484ULL,   0x140311c080c00022ULL, 0x1004082010400040ULL, 0x110002400200400ULL,
  0x40a02240200800ULL,   0x70480080d09040ULL,   0x1083000100800204ULL, 0x40042a510020006ULL,  0x8261014280380100ULL,
  0x24402210042000ULL,   0x808000841001810ULL,  0x2000800811420004ULL, 0x8050006140100100ULL, 0x8002040100200dULL,
  0x40080008100ULL,      0xa004108004204002ULL, 0xaa100429009ULL,      0x104000802080ULL,     0x400922050090118ULL,
  0x810820104c25200ULL,  0x290005902801001ULL,  0x208080100810010aULL, 0x5004400011240ULL,    0x8104032040ULL,
  0x1002000a0426420ULL,  0x82042081041042ULL,   0x8040c2900804001ULL,  0x884401004200882ULL,  0x88040220a000402ULL,
  0x2303800644201ULL,    0x86e000980c122ULL,    0x82220a0461048204ULL, 0x1104010024104082ULL,
};

const std::array<uint64_t, 64> c_bishop_magics{
  0x4010002202254010ULL, 0x4000506049214012ULL, 0xc41200304280c044ULL, 0x8a2444c808010008ULL, 0x4020484080440000ULL,
  0x86088240010401ULL,   0x204024041988805ULL,  0x121235021204012ULL,  0x8004024c10108a00ULL, 0x4c0060220023ULL,
  0x20105844b4000ULL,    0xa0346880041000ULL,   0x225028000009ULL,     0x44010144025ULL,      0x10048044002224ULL,
  0xc080a01403010021ULL, 0x4005000068002041ULL, 0x810000062002511ULL,  0x4440210104804484ULL, 0x2094000820401000ULL,
  0x4024000080214001ULL, 0x488080800820020cULL, 0x210100118410a820ULL, 0x1d02104483a600b0ULL, 0x368201006002200ULL,
  0x120262000800430cULL, 0x1914008001000900ULL, 0x86c080014006088ULL,  0x812002202008040ULL,  0x13410020040c2ULL,
  0x80008008010008c0ULL, 0x20a004040050090ULL,  0x40100400120480ULL,   0x8011001064029428ULL, 0x404d0c02a140100ULL,
  0x40020080080080ULL,   0x40040120c0100ULL,    0x44620040a032ULL,     0x8204610810002c32ULL, 0x12204034a400ULL,
  0xa24a40a000600ULL,    0x1080401a0420810ULL,  0x103041010500200ULL,  0x8000029088002070ULL, 0x80048100420400ULL,
  0x521041000080382ULL,  0x802886021808c040ULL, 0x1002842408080084ULL, 0x80c41040414ULL,      0x516118080102000ULL,
  0x20008040130020ULL,   0x684044040ULL,        0x29002000c5100920ULL, 0x806004944406ULL,     0x8410002140d40088ULL,
  0x1060032c0408eULL,    0x8000023202014000ULL, 0x102001008821081ULL,  0x706418104102c08ULL,  0x1064008000034400ULL,
  0x110018809820080ULL,  0x462824088020422ULL,  0x88600e210a00a020ULL, 0x6011008100188200ULL,
};

constexpr void update_if_in_bounds_(Bitboard& bb, int x, int y)
{
  if (0 <= x && x < c_board_dimension && 0 <= y && y < c_board_dimension)
  {
    bb.set_square(Coordinates{x, y}.square_index());
  }
}

// The following functions rely on colors casting to these values
static_assert(static_cast<uint8_t>(Color::black) == 0);
static_assert(static_cast<uint8_t>(Color::white) == 1);

using shift_fn = Bitboard (Bitboard::*)(int32_t) const;

// This array can be indexed by Color, so the operator to be used for black pawns is first
constexpr std::array c_shift_functions{&Bitboard::operator>>, &Bitboard::operator<< };
constexpr shift_fn get_pawn_shift_fn(Color color)
{
  return c_shift_functions[static_cast<int32_t>(color)];
}

// This array can be indexed by Color, so the operator to be used for black pawns is first
constexpr std::array c_start_ranks{Bitboard_constants::seventh_rank, Bitboard_constants::second_rank};
constexpr Bitboard get_pawn_start_rank(Color color)
{
  return c_start_ranks[static_cast<int32_t>(color)];
}

constexpr std::array c_square_offsets{c_board_dimension, -c_board_dimension};
constexpr int32_t get_start_square_offset(Color color)
{
  return c_square_offsets[static_cast<int32_t>(color)];
}

constexpr std::array c_east_shift_offsets{7, 9};
constexpr int32_t get_east_shift_distance(Color color)
{
  return c_east_shift_offsets[static_cast<int32_t>(color)];
}

constexpr std::array c_west_shift_offsets{9, 7};
constexpr int32_t get_west_shift_distance(Color color)
{
  return c_west_shift_offsets[static_cast<int32_t>(color)];
}

constexpr std::array c_east_offsets{7, -9};
constexpr int32_t get_east_capture_offset(Color color)
{
  return c_east_offsets[static_cast<int32_t>(color)];
}

constexpr std::array c_west_offsets{9, -7};
constexpr int32_t get_west_capture_offset(Color color)
{
  return c_west_offsets[static_cast<int32_t>(color)];
}

#if 0
int count_1s(uint64_t b)
{
  int r{0};
  for (; b; r++, b &= b - 1)
    ;
  return r;
}
#endif

const std::array<int, 64> BitTable{63, 30, 3,  32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34,
                                   61, 29, 2,  51, 21, 43, 45, 10, 18, 47, 1,  54, 9,  57, 0,  35,
                                   62, 31, 40, 4,  49, 5,  52, 26, 60, 6,  23, 44, 46, 27, 56, 16,
                                   7,  39, 48, 24, 59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8};

int pop_1st_bit(uint64_t* bb)
{
  uint64_t b = *bb ^ (*bb - 1);
  auto fold = static_cast<uint32_t>((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return BitTable[(fold * 0x783a9b23) >> 26];
}

uint64_t blocker_permutation_from_index(int index, int bits, uint64_t m)
{
  uint64_t result = 0ULL;
  for (int i = 0; i < bits; i++)
  {
    int j = pop_1st_bit(&m);
    if (index & (1 << i))
    {
      result |= (1ULL << j);
    }
  }
  return result;
}

Bitboard rook_potential_blockers(int sq)
{
  Bitboard result{0ULL};
  int rk = sq / 8;
  int fl = sq % 8;
  for (int r = rk + 1; r <= 6; r++)
    result |= (Bitboard{1ULL} << (fl + r * 8));
  for (int r = rk - 1; r >= 1; r--)
    result |= (Bitboard{1ULL} << (fl + r * 8));
  for (int f = fl + 1; f <= 6; f++)
    result |= (Bitboard{1ULL} << (f + rk * 8));
  for (int f = fl - 1; f >= 1; f--)
    result |= (Bitboard{1ULL} << (f + rk * 8));
  return result;
}

Bitboard bishop_potential_blockers(int sq)
{
  Bitboard result{0ULL};
  int rank = sq / 8;
  int file = sq % 8;
  for (int r = rank + 1, f = file + 1; r <= 6 && f <= 6; r++, f++)
  {
    result |= (Bitboard{1ULL} << (f + r * 8));
  }
  for (int r = rank + 1, f = file - 1; r <= 6 && f >= 1; r++, f--)
  {
    result |= (Bitboard{1ULL} << (f + r * 8));
  }
  for (int r = rank - 1, f = file + 1; r >= 1 && f <= 6; r--, f++)
  {
    result |= (Bitboard{1ULL} << (f + r * 8));
  }
  for (int r = rank - 1, f = file - 1; r >= 1 && f >= 1; r--, f--)
  {
    result |= (Bitboard{1ULL} << (f + r * 8));
  }
  return result;
}

Bitboard rook_attacked_squares(int sq, uint64_t block)
{
  Bitboard result{0ULL};
  int rk = sq / 8, fl = sq % 8;
  for (int r = rk + 1; r <= 7; r++)
  {
    result |= (Bitboard{1ULL} << (fl + r * 8));
    if (block & (1ULL << (fl + r * 8)))
      break;
  }
  for (int r = rk - 1; r >= 0; r--)
  {
    result |= (Bitboard{1ULL} << (fl + r * 8));
    if (block & (1ULL << (fl + r * 8)))
      break;
  }
  for (int f = fl + 1; f <= 7; f++)
  {
    result |= (Bitboard{1ULL} << (f + rk * 8));
    if (block & (1ULL << (f + rk * 8)))
      break;
  }
  for (int f = fl - 1; f >= 0; f--)
  {
    result |= (Bitboard{1ULL} << (f + rk * 8));
    if (block & (1ULL << (f + rk * 8)))
      break;
  }
  return result;
}

Bitboard bishop_attacked_squares(int sq, uint64_t block)
{
  Bitboard result{0ULL};
  int rk = sq / 8, fl = sq % 8;
  for (int r = rk + 1, f = fl + 1; r <= 7 && f <= 7; r++, f++)
  {
    result |= (Bitboard{1ULL} << (f + r * 8));
    if (block & (1ULL << (f + r * 8)))
      break;
  }
  for (int r = rk + 1, f = fl - 1; r <= 7 && f >= 0; r++, f--)
  {
    result |= (Bitboard{1ULL} << (f + r * 8));
    if (block & (1ULL << (f + r * 8)))
      break;
  }
  for (int r = rk - 1, f = fl + 1; r >= 0 && f <= 7; r--, f++)
  {
    result |= (Bitboard{1ULL} << (f + r * 8));
    if (block & (1ULL << (f + r * 8)))
      break;
  }
  for (int r = rk - 1, f = fl - 1; r >= 0 && f >= 0; r--, f--)
  {
    result |= (Bitboard{1ULL} << (f + r * 8));
    if (block & (1ULL << (f + r * 8)))
      break;
  }
  MY_ASSERT(result != Bitboard{0}, "Cannot have a bishop with no attacked squares");
  return result;
}

int magic_hash_fn(uint64_t blockers, uint64_t magic, int bits)
{
  MY_ASSERT(bits == 9         || bits==12, "Fixed shift");
  return (int)((blockers * magic) >> (64 - bits));
}

} // namespace

Move_generator::Tables const Move_generator::m_tables{};

Move_generator::Tables::Tables()
{
  initialize_ray_attacks_();
  initialize_knight_attacks_();
  initialize_king_attacks_();
}

void Move_generator::Tables::initialize_ray_attacks_()
{
  for (int8_t sq{0}; sq < c_board_dimension_squared; ++sq)
  {
    auto const index = sq;
    for (Compass_dir dir = Compass_dir::north; dir < Compass_dir::_count; ++dir)
    {
      auto const coords = Coordinates{index};
      Bitboard& bb = ray_attacks[index][dir];

      int32_t square_x{coords.x()};
      int32_t square_y{coords.y()};
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

    init_bishop_magic_tables_(index);
    init_rook_magic_tables_(index);
  } // for each index
} // initialize_ray_attacks

void Move_generator::Tables::init_bishop_magic_tables_(int index)
{
  bool bishop = true;
  Bitboard possible_blockers = bishop ? bishop_potential_blockers(index) : rook_potential_blockers(index);

  bishop_magic_table[index] = Tables::Magic{possible_blockers, c_bishop_magics[index]};

  if (is_debug && index == debug_square_index)
  {
    std::cout << "c_bishop_magics.size: " << c_bishop_magics.size() << "\n";
    std::cout << "Possible bishop blockers\n" << possible_blockers << "\n";
  }

  // Populate blockers and attackers table
  int n = possible_blockers.occupancy();
  auto blocker_permutations = (1 << n);
  std::set<uint64_t> blockers_set;
  std::set<int> key_set;
  int empty_blockers_count{0};

  for (int i = 0; i < blocker_permutations; ++i)
  {
    MY_ASSERT(bishop_attacks[index][i] == Bitboard{0}, "Array should start zero initialized");
  }

  for (int i = 0; i < blocker_permutations; ++i)
  {
    if (is_debug)
    {
      std::cout << "Setting square: " << std::to_string(index) << ", permutation: " << i << "\n";
    }
    auto blockers = blocker_permutation_from_index(i, n, possible_blockers.val);
    if (Bitboard{blockers}.is_empty())
    {
      empty_blockers_count++;
    }
    MY_ASSERT(empty_blockers_count <= 1, "One square should have zero blockers");

    MY_ASSERT(!blockers_set.contains(blockers), "Permutation code is broken");
    blockers_set.insert(blockers);

    //TODO: Do we still need BBits & RBits?
    int shift = bishop ? 9 : 12;
    auto key = magic_hash_fn(blockers, c_bishop_magics[index], shift);

    auto attacked_squares = bishop_attacked_squares(index, blockers);

    if (key_set.contains(key))
    {
      if (is_debug)
      {
        std::cout << "Duplicate key: " << key << " at index: " << std::to_string(index) << " and permutation: " << i
                  << "\n";
      }
      MY_ASSERT(!key_set.contains(key), "Duplicate key");
    }
    if (static_cast<size_t>(key) >= bishop_attacks.front().size() && is_debug)
    {
      std::cout << "Key out of range: " << key << "\n";
    }
    MY_ASSERT(static_cast<size_t>(key) < bishop_attacks.front().size(), "Key out of range");

    key_set.insert(key);

    if (is_debug)
    {
      if (index == debug_square_index)
      {
        std::cout << "Possible_blockers:\n" << possible_blockers;
        std::cout << "Attacked squares:\n" << attacked_squares;
        std::cout << "Index: " << index << "\n";
        std::cout << "Key: " << key << "\n";
      }

      if (index == debug_square_index && key == 53)
      {
        static int times_set{1};
        std::cout << "Times setting bishop_attacks[" << debug_square_index << "][53]: " << times_set++ << "\n";
      }
    }

    if (is_debug && bishop_attacks[index][key] != Bitboard{0})
    {
      std::cout << "Setting already set bitboard. Square index: " << std::to_string(index) << ", key: " << key << "\n"
                << Bitboard{blockers} << "\n";
    }
    MY_ASSERT(bishop_attacks[index][key] == Bitboard{0}, "Setting already set bitboard");
    bishop_attacks[index][key] = attacked_squares; // Permutations?
    //attacked[i] = bishop ? bishop_attacked_squares(index, blockers[i]) : rook_attacked_squares(index, blockers[i]);
  }
}

void Move_generator::Tables::init_rook_magic_tables_(int index)
{
  bool bishop = false;
  Bitboard possible_blockers = bishop ? bishop_potential_blockers(index) : rook_potential_blockers(index);

  rook_magic_table[index] = Tables::Magic{possible_blockers, c_rook_magics[index]};

  if (is_debug && index == debug_square_index)
  {
    std::cout << "c_rook_magics.size: " << c_rook_magics.size() << "\n";
    std::cout << "Possible rook blockers\n" << possible_blockers << "\n";
  }

  // Populate blockers and attackers table
  int n = possible_blockers.occupancy();
  auto blocker_permutations = (1 << n);
  std::set<uint64_t> blockers_set;
  std::set<int> key_set;
  int empty_blockers_count{0};

  for (int i = 0; i < blocker_permutations; ++i)
  {
    MY_ASSERT(rook_attacks[index][i] == Bitboard{0}, "Array should start zero initialized");
  }

  for (int i = 0; i < blocker_permutations; ++i)
  {
    if (is_debug)
    {
      std::cout << "Setting square: " << std::to_string(index) << ", permutation: " << i << "\n";
    }
    auto blockers = blocker_permutation_from_index(i, n, possible_blockers.val);
    if (Bitboard{blockers}.is_empty())
    {
      empty_blockers_count++;
    }
    MY_ASSERT(empty_blockers_count <= 1, "One square should have zero blockers");

    MY_ASSERT(!blockers_set.contains(blockers), "Permutation code is broken");
    blockers_set.insert(blockers);

    //TODO: Do we still need BBits & RBits?
    int shift = bishop ? 9 : 12;
    auto key = magic_hash_fn(blockers, c_rook_magics[index], shift);

    auto attacked_squares = rook_attacked_squares(index, blockers);

    if (key_set.contains(key))
    {
      if (is_debug)
      {
        std::cout << "Duplicate key: " << key << " at index: " << std::to_string(index) << " and permutation: " << i
                  << "\n";
      }
      MY_ASSERT(!key_set.contains(key), "Duplicate key");
    }
    if (static_cast<size_t>(key) >= rook_attacks.front().size() && is_debug)
    {
      std::cout << "Key out of range: " << key << "\n";
    }
    MY_ASSERT(static_cast<size_t>(key) < rook_attacks.front().size(), "Key out of range");

    key_set.insert(key);

    if (is_debug)
    {
      if (index == debug_square_index)
      {
        std::cout << "Possible_blockers:\n" << possible_blockers;
        std::cout << "Attacked squares:\n" << attacked_squares;
        std::cout << "Index: " << index << "\n";
        std::cout << "Key: " << key << "\n";
      }

      if (index == debug_square_index && key == 53)
      {
        static int times_set{1};
        std::cout << "Times setting rook_attacks[" << debug_square_index << "][53]: " << times_set++ << "\n";
      }
    }

    if (is_debug && rook_attacks[index][key] != Bitboard{0})
    {
      std::cout << "Setting already set bitboard. Square index: " << std::to_string(index) << ", key: " << key << "\n"
                << Bitboard{blockers} << "\n";
    }
    MY_ASSERT(rook_attacks[index][key] == Bitboard{0}, "Setting already set bitboard");
    rook_attacks[index][key] = attacked_squares; // Permutations?
    //attacked[i] = rook ? rook_attacked_squares(index, blockers[i]) : rook_attacked_squares(index, blockers[i]);
  }
}

void Move_generator::Tables::initialize_knight_attacks_()
{
  for (int8_t x{0}; x < c_board_dimension; ++x)
  {
    for (int8_t y{0}; y < c_board_dimension; ++y)
    {
      auto square_index = Coordinates{x, y}.square_index();
      auto& bb = knight_attacks[square_index];

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

void Move_generator::Tables::initialize_king_attacks_()
{
  for (int8_t x{0}; x < c_board_dimension; ++x)
  {
    for (int8_t y{0}; y < c_board_dimension; ++y)
    {
      auto square_index = Coordinates{x, y}.square_index();
      auto& bb = king_attacks[square_index];

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

#if 1
Bitboard Move_generator::gen_rook_attacks_magic(Coordinates coords, Bitboard occupied)
{
  int index = coords.square_index();
  occupied &= m_tables.rook_magic_table[coords.square_index()].mask;

  auto key = magic_hash_fn(occupied.val, c_rook_magics[index], 12);

  auto& t = m_tables.rook_attacks[index];
  auto result = t[key];
  if (is_debug)
  {
    std::cout << "2Relevant_blockers:\n" << occupied;
    std::cout << "2Attacked squares:\n" << result;
    std::cout << "2Index: " << index << "\n";
    std::cout << "2Key: " << key << "\n";
  }
  return result;
}

Bitboard Move_generator::gen_bishop_attacks_magic(Coordinates coords, Bitboard occupied)
{
  int index = coords.square_index();
  occupied &= m_tables.bishop_magic_table[coords.square_index()].mask;

  auto key = magic_hash_fn(occupied.val, c_bishop_magics[index], 9);

  //occupied *= m_tables.bishop_magic_table[coords.square_index()].magic;
  //occupied >>= (64-BBits[coords.square_index()]);
  auto& t = m_tables.bishop_attacks[index];
  auto result = t[key];
  if (is_debug)
  {
    std::cout << "2Relevant_blockers:\n" << occupied;
    std::cout << "2Attacked squares:\n" << result;
    std::cout << "2Index: " << index << "\n";
    std::cout << "2Key: " << key << "\n";
  }
  return result;
}
#endif

Bitboard Move_generator::get_positive_ray_attacks_(Coordinates square, Compass_dir dir, Bitboard occupied)
{
  MY_ASSERT(dir.is_positive(), "Only positive directions are supported");

  Bitboard attacks = m_tables.ray_attacks[square.square_index()][dir];
  Bitboard blockers = attacks & occupied;
  if (!blockers.is_empty())
  {
    auto const first_blocker = blockers.bitscan_forward();
    attacks ^= m_tables.ray_attacks[first_blocker][dir];
  }

  return attacks;
}

Bitboard Move_generator::get_negative_ray_attacks_(Coordinates square, Compass_dir dir, Bitboard occupied)
{
  MY_ASSERT(!dir.is_positive(), "Only negative directions are supported");

  Bitboard attacks = m_tables.ray_attacks[square.square_index()][dir];
  Bitboard blockers = attacks & occupied;
  if (!blockers.is_empty())
  {
    auto const first_blocker = blockers.bitscan_reverse();
    attacks ^= m_tables.ray_attacks[first_blocker][dir];
  }

  return attacks;
}

Bitboard Move_generator::gen_sliding_moves_(std::span<const Compass_dir> positive_directions,
                                            std::span<const Compass_dir> negative_directions,
                                            Coordinates square,
                                            Bitboard occupied)
{
  auto positive_attacks = std::accumulate(positive_directions.begin(), positive_directions.end(), Bitboard{0},
                                          [&](Bitboard result, Compass_dir dir)
                                          {
                                            return result | get_positive_ray_attacks_(square, dir, occupied);
                                          });
  return std::accumulate(negative_directions.begin(), negative_directions.end(), positive_attacks,
                         [&](Bitboard result, Compass_dir dir)
                         {
                           return result | get_negative_ray_attacks_(square, dir, occupied);
                         });
}

Bitboard Move_generator::rook_attacks(Coordinates square, Bitboard occupied)
{
  constexpr static std::array<Compass_dir, 2> positive_directions{Compass_dir::north, Compass_dir::east};
  constexpr static std::array<Compass_dir, 2> negative_directions{Compass_dir::south, Compass_dir::west};

  auto standard_result = gen_sliding_moves_(positive_directions, negative_directions, square, occupied);
  auto magic_result = gen_rook_attacks_magic(square, occupied);
  if (standard_result != magic_result)
  {
    std::cout << "Mismatched attack sets for bishop on " << square << " (index: " << square.square_index() << ")\n";
    std::cout << "Occupied squares:\n" << occupied << "\n";
    std::cout << "Standard attack set:\n" << standard_result << "\nMagic attack set:\n" << magic_result << "\n";
    exit(-2);
  }
  return magic_result;
}

Bitboard Move_generator::bishop_attacks(Coordinates square, Bitboard occupied)
{
  constexpr static std::array<Compass_dir, 2> positive_directions{Compass_dir::northeast, Compass_dir::northwest};
  constexpr static std::array<Compass_dir, 2> negative_directions{Compass_dir::southeast, Compass_dir::southwest};

  auto standard_result = gen_sliding_moves_(positive_directions, negative_directions, square, occupied);

  auto magic_result = gen_bishop_attacks_magic(square, occupied);
  if (standard_result != magic_result)
  {
    std::cout << "Mismatched attack sets for bishop on " << square << " (index: " << square.square_index() << ")\n";
    std::cout << "Occupied squares:\n" << occupied << "\n";
    std::cout << "Standard attack set:\n" << standard_result << "\nMagic attack set:\n" << magic_result << "\n";
    exit(-2);
  }
  return magic_result;
}

Bitboard Move_generator::queen_attacks(Coordinates square, Bitboard occupied)
{
  return bishop_attacks(square, occupied) | rook_attacks(square, occupied);
}

Bitboard Move_generator::knight_attacks(Coordinates square, Bitboard /* occupied */)
{
  return m_tables.knight_attacks[square.square_index()];
}

Bitboard Move_generator::king_attacks(Coordinates square, Bitboard /* occupied */)
{
  return m_tables.king_attacks[square.square_index()];
}

Bitboard Move_generator::pawn_short_advances(Color color, Bitboard pawns, Bitboard occupied)
{
  auto const bitshift_fn = get_pawn_shift_fn(color);
  auto const one_space_moves = (pawns.*bitshift_fn)(c_board_dimension) & ~occupied;
  return one_space_moves;
}

Bitboard Move_generator::pawn_long_advances(Color color, Bitboard pawns, Bitboard occupied)
{
  auto const bitshift_fn = get_pawn_shift_fn(color);
  auto const start_rank = get_pawn_start_rank(color);
  auto const eligible_pawns = pawns & start_rank;
  auto const one_space_moves = (eligible_pawns.*bitshift_fn)(c_board_dimension) & ~occupied;
  auto const two_space_moves = (one_space_moves.*bitshift_fn)(c_board_dimension) & ~occupied;
  return two_space_moves;
}

Bitboard Move_generator::pawn_potential_attacks(Color color, Bitboard pawns)
{
  auto const bitshift_fn = get_pawn_shift_fn(color);
  auto const shift_distance_east = get_east_shift_distance(color); // new
  auto const shift_distance_west = get_west_shift_distance(color); // new
  auto const west_attacks = (pawns.*bitshift_fn)(shift_distance_west) & ~Bitboard_constants::h_file;

  auto const east_attacks = (pawns.*bitshift_fn)(shift_distance_east) & ~Bitboard_constants::a_file;
  return west_attacks | east_attacks;
}

Bitboard Move_generator::pawn_east_attacks(Color color, Bitboard pawns, Bitboard enemies)
{
  auto const bitshift_fn = get_pawn_shift_fn(color);
  auto const shift_distance = get_east_shift_distance(color); // new
  auto const east_attacks = (pawns.*bitshift_fn)(shift_distance) & ~Bitboard_constants::a_file & enemies;
  return east_attacks;
}

Bitboard Move_generator::pawn_west_attacks(Color color, Bitboard pawns, Bitboard enemies)
{
  auto const bitshift_fn = get_pawn_shift_fn(color);
  auto const shift_distance = get_west_shift_distance(color); // new
  auto const west_attacks = (pawns.*bitshift_fn)(shift_distance) & ~Bitboard_constants::h_file & enemies;
  return west_attacks;
}

void Move_generator::generate_castling_moves(Board const& board, Color color, std::vector<Move>& moves)
{
  static constexpr Move short_castle_white{{4, 0}, {6, 0}, Piece::king, Piece::empty};
  static constexpr Move long_castle_white{{4, 0}, {2, 0}, Piece::king, Piece::empty};
  static constexpr Move short_castle_black{{4, 7}, {6, 7}, Piece::king, Piece::empty};
  static constexpr Move long_castle_black{{4, 7}, {2, 7}, Piece::king, Piece::empty};

  auto const attacks = Move_generator::get_all_attacked_squares(board, opposite_color(color));
  auto const castling_rights = board.get_castling_rights();

  int count{0};

  if (color == Color::white)
  {
    if ((attacks & Bitboard_constants::short_castling_empty_squares_white).is_empty() &&
        (attacks & board.get_piece_set(color, Piece::king)).is_empty() &&
        (board.get_occupied_squares() & Bitboard_constants::short_castling_empty_squares_white).is_empty() &&
        castling_rights.white_can_short_castle)
    {
      moves.emplace_back(short_castle_white);
      ++count;
    }

    if ((attacks & Bitboard_constants::long_castling_empty_squares_white).is_empty() &&
        (attacks & board.get_piece_set(color, Piece::king)).is_empty() &&
        (board.get_occupied_squares() & Bitboard_constants::long_castling_empty_squares_white).is_empty() &&
        !board.get_occupied_squares().is_set(Coordinates{1, 0}) && castling_rights.white_can_long_castle)
    {
      moves.emplace_back(long_castle_white);
      ++count;
    }
  }
  else
  {
    if ((attacks & Bitboard_constants::short_castling_empty_squares_black).is_empty() &&
        (attacks & board.get_piece_set(color, Piece::king)).is_empty() &&
        (board.get_occupied_squares() & Bitboard_constants::short_castling_empty_squares_black).is_empty() &&
        castling_rights.black_can_short_castle)
    {
      moves.emplace_back(short_castle_black);
      ++count;
    }

    if ((attacks & Bitboard_constants::long_castling_empty_squares_black).is_empty() &&
        (attacks & board.get_piece_set(color, Piece::king)).is_empty() &&
        (board.get_occupied_squares() & Bitboard_constants::long_castling_empty_squares_black).is_empty() &&
        !board.get_occupied_squares().is_set(Coordinates{1, 7}) && castling_rights.black_can_long_castle)
    {
      moves.emplace_back(long_castle_black);
      ++count;
    }
  }
}

void Move_generator::generate_piece_moves(Board const& board, Color color, std::vector<Move>& moves)
{
  // Parallel arrays that can be iterated together to get the piece type and the function that matches it
  constexpr static std::array piece_types{Piece::rook, Piece::knight, Piece::bishop, Piece::queen, Piece::king};
  constexpr static std::array piece_move_functions{&Move_generator::rook_attacks, &Move_generator::knight_attacks,
                                                   &Move_generator::bishop_attacks, &Move_generator::queen_attacks,
                                                   &Move_generator::king_attacks};
  for (size_t i{0}; i < piece_types.size(); ++i)
  {
    for (auto piece_location : board.get_piece_set(color, piece_types[i]))
    {
      auto possible_moves = piece_move_functions[i](Coordinates{piece_location}, board.get_occupied_squares());
      possible_moves &= ~board.get_all(color); // Throw out any moves to a square that is already occupied by our color
      auto const possible_attacks = possible_moves & board.get_all(opposite_color(color));
      possible_moves &= ~possible_attacks; // Handle attacks separately
      for (auto end_location : possible_moves)
      {
        moves.emplace_back(Coordinates{piece_location}, Coordinates{end_location}, piece_types[i], Piece::empty);
      }
      for (auto end_location : possible_attacks)
      {
        moves.emplace_back(Coordinates{piece_location}, Coordinates{end_location}, piece_types[i],
                           board.get_piece(Coordinates{end_location}));
      }
    }
  }
}

void Move_generator::generate_piece_attacks(Board const& board, Color color, std::vector<Move>& moves)
{
  // Parallel arrays that can be iterated together to get the piece type and the function that matches it
  constexpr static std::array piece_types{Piece::rook, Piece::knight, Piece::bishop, Piece::queen, Piece::king};
  constexpr static std::array piece_move_functions{&Move_generator::rook_attacks, &Move_generator::knight_attacks,
                                                   &Move_generator::bishop_attacks, &Move_generator::queen_attacks,
                                                   &Move_generator::king_attacks};
  for (size_t i{0}; i < piece_types.size(); ++i)
  {
    for (auto piece_location : board.get_piece_set(color, piece_types[i]))
    {
      auto attacks = piece_move_functions[i](Coordinates{piece_location}, board.get_occupied_squares());
      attacks &= ~board.get_all(color); // Throw out any moves to a square that is already occupied by our color
      attacks &= board.get_all(opposite_color(color)); // Throw out any moves that are not captures
      for (auto end_location : attacks)
      {
        moves.emplace_back(Coordinates{piece_location}, Coordinates{end_location}, piece_types[i],
                           board.get_piece(Coordinates{end_location}));
      }
    }
  }
}

void Move_generator::generate_pawn_attacks(Board const& board, Color color, std::vector<Move>& moves)
{
  // Handle east captures
  auto const east_offset = get_east_capture_offset(color);
  for (auto location :
       pawn_east_attacks(color, board.get_piece_set(color, Piece::pawn), board.get_all(opposite_color(color))))
  {
    Coordinates from{location + east_offset};
    Coordinates to{location};
    auto const victim = board.get_piece(to);
    if (to.y() == 0 || to.y() == 7)
    {
      moves.emplace_back(from, to, Piece::pawn, victim, Piece::bishop);
      moves.emplace_back(from, to, Piece::pawn, victim, Piece::knight);
      moves.emplace_back(from, to, Piece::pawn, victim, Piece::rook);
      moves.emplace_back(from, to, Piece::pawn, victim, Piece::queen);
    }
    else
    {
      moves.emplace_back(from, to, Piece::pawn, victim);
    }
  }

  // Handle west captures
  auto const west_offset = get_west_capture_offset(color);
  for (auto location :
       pawn_west_attacks(color, board.get_piece_set(color, Piece::pawn), board.get_all(opposite_color(color))))
  {
    Coordinates from{location + west_offset};
    Coordinates to{location};
    auto victim = board.get_piece(to);
    if (to.y() == 0 || to.y() == 7)
    {
      moves.emplace_back(from, to, Piece::pawn, victim, Piece::bishop);
      moves.emplace_back(from, to, Piece::pawn, victim, Piece::knight);
      moves.emplace_back(from, to, Piece::pawn, victim, Piece::rook);
      moves.emplace_back(from, to, Piece::pawn, victim, Piece::queen);
    }
    else
    {
      moves.emplace_back(from, to, Piece::pawn, victim);
    }
  }
}

void Move_generator::generate_pawn_moves(Board const& board, Color color, std::vector<Move>& moves)
{
  // For a one square pawn push, the starting square will be either 8 squares higher or lower than the ending square
  auto offset_from_end_square = get_start_square_offset(color);

  for (auto location :
       pawn_short_advances(color, board.get_piece_set(color, Piece::pawn), board.get_occupied_squares()))
  {
    Coordinates from{location + offset_from_end_square};
    Coordinates to{location};
    if (to.y() == 0 || to.y() == 7)
    {
      moves.emplace_back(from, to, Piece::pawn, Piece::empty, Piece::bishop);
      moves.emplace_back(from, to, Piece::pawn, Piece::empty, Piece::knight);
      moves.emplace_back(from, to, Piece::pawn, Piece::empty, Piece::rook);
      moves.emplace_back(from, to, Piece::pawn, Piece::empty, Piece::queen);
    }
    else
    {
      moves.emplace_back(from, to, Piece::pawn, Piece::empty);
    }
  }

  // Handle long advances
  for (auto location : pawn_long_advances(color, board.get_piece_set(color, Piece::pawn), board.get_occupied_squares()))
  {
    moves.emplace_back(Coordinates{location + 2 * offset_from_end_square}, Coordinates{location}, Piece::pawn,
                       Piece::empty);
  }

  auto const east_offset = get_east_capture_offset(color);
  // Handle en passant
  for (auto location : pawn_east_attacks(color, board.get_piece_set(color, Piece::pawn), board.get_en_passant_square()))
  {
    moves.emplace_back(Coordinates{location + east_offset}, Coordinates{location}, Piece::pawn, Piece::pawn,
                       Piece::empty, Move_type::en_passant);
  }

  auto const west_offset = get_west_capture_offset(color);
  for (auto location : pawn_west_attacks(color, board.get_piece_set(color, Piece::pawn), board.get_en_passant_square()))
  {
    moves.emplace_back(Coordinates{location + west_offset}, Coordinates{location}, Piece::pawn, Piece::pawn,
                       Piece::empty, Move_type::en_passant);
  }

  generate_pawn_attacks(board, color, moves);
}

std::vector<Move> Move_generator::generate_pseudo_legal_moves(Board const& board)
{
  auto const color = board.get_active_color();
  std::vector<Move> pseudo_legal_moves;
  pseudo_legal_moves.reserve(218); // Max possible number of chess moves in a position

  generate_pawn_moves(board, color, pseudo_legal_moves);
  generate_piece_moves(board, color, pseudo_legal_moves);
  generate_castling_moves(board, color, pseudo_legal_moves);

  return pseudo_legal_moves;
}

std::vector<Move> Move_generator::generate_legal_moves(Board const& board)
{
  auto const color = board.get_active_color();
  std::vector<Move> pseudo_legal_moves;
  pseudo_legal_moves.reserve(218); // Max possible number of chess moves in a position

  generate_pawn_moves(board, color, pseudo_legal_moves);
  generate_piece_moves(board, color, pseudo_legal_moves);
  generate_castling_moves(board, color, pseudo_legal_moves);

  std::vector<Move> legal_moves;
  legal_moves.reserve(pseudo_legal_moves.size());
  Board tmp_board(board);
  std::copy_if(pseudo_legal_moves.cbegin(), pseudo_legal_moves.cend(), std::back_inserter(legal_moves),
               [&](auto m)
               {
                 tmp_board = board;
                 return !tmp_board.move_results_in_check_destructive(m);
               });

  return legal_moves;
}

std::vector<Move> Move_generator::generate_legal_attack_moves(Board const& board)
{
  auto const color = board.get_active_color();
  std::vector<Move> pseudo_legal_attacks;
  pseudo_legal_attacks.reserve(64);

  generate_piece_attacks(board, color, pseudo_legal_attacks);
  generate_pawn_attacks(board, color, pseudo_legal_attacks);

  std::vector<Move> legal_attacks;
  legal_attacks.reserve(64);
  Board tmp_board(board);
  std::copy_if(pseudo_legal_attacks.cbegin(), pseudo_legal_attacks.cend(), std::back_inserter(legal_attacks),
               [&](auto m)
               {
                 tmp_board = board;
                 return !tmp_board.move_results_in_check_destructive(m);
               });

  return legal_attacks;
}

Bitboard Move_generator::get_all_attacked_squares(Board const& board, Color attacking_color)
{
  constexpr static std::array piece_types{Piece::rook, Piece::knight, Piece::bishop, Piece::queen, Piece::king};
  constexpr static std::array piece_move_functions{&Move_generator::rook_attacks, &Move_generator::knight_attacks,
                                                   &Move_generator::bishop_attacks, &Move_generator::queen_attacks,
                                                   &Move_generator::king_attacks};
  Bitboard attacked_squares;
  for (size_t i{0}; i < piece_types.size(); ++i)
  {
    for (auto piece_location : board.get_piece_set(attacking_color, piece_types[i]))
    {
      auto attacks = piece_move_functions[i](Coordinates{piece_location}, board.get_occupied_squares());
      attacked_squares |= attacks;
    }
  }

  return attacked_squares | pawn_potential_attacks(attacking_color, board.get_piece_set(attacking_color, Piece::pawn));
}

// Faster than generating all moves and checking if the list is empty
bool Move_generator::has_any_legal_moves(Board const& board)
{
  // Parallel arrays that can be iterated together to get the piece type and the function that matches it
  constexpr static std::array piece_types{Piece::king, Piece::queen, Piece::knight, Piece::bishop, Piece::rook};
  constexpr static std::array piece_move_functions{&Move_generator::king_attacks, &Move_generator::queen_attacks,
                                                   &Move_generator::knight_attacks, &Move_generator::bishop_attacks,
                                                   &Move_generator::rook_attacks};

  Board tmp_board{board};
  auto const color = board.get_active_color();
  for (size_t i{0}; i < piece_types.size(); ++i)
  {
    for (auto piece_location : board.get_piece_set(color, piece_types[i]))
    {
      auto possible_moves = piece_move_functions[i](Coordinates{piece_location}, board.get_occupied_squares());
      possible_moves &= ~board.get_all(color); // Throw out any moves to a square that is already occupied by our color
      auto const possible_attacks = possible_moves & board.get_all(opposite_color(color));
      possible_moves &= ~possible_attacks; // Handle attacks separately

      for (auto end_location : possible_moves)
      {
        tmp_board = board;
        if (!tmp_board.move_results_in_check_destructive(
              {Coordinates{piece_location}, Coordinates{end_location}, piece_types[i], Piece::empty}))
        {
          return true;
        }
      }

      for (auto end_location : possible_attacks)
      {
        tmp_board = board;
        if (!tmp_board.move_results_in_check_destructive({Coordinates{piece_location}, Coordinates{end_location},
                                                          piece_types[i], board.get_piece(Coordinates{end_location})}))
        {
          return true;
        }
      }
    }
  }

  std::vector<Move> pseudo_legal_moves;
  pseudo_legal_moves.reserve(218); // Max possible number of chess moves in a position

  generate_pawn_moves(board, color, pseudo_legal_moves);
  return std::any_of(pseudo_legal_moves.cbegin(), pseudo_legal_moves.cend(),
                     [&](auto m)
                     {
                       tmp_board = board;
                       return !tmp_board.move_results_in_check_destructive(m);
                     });
}

uint64_t find_magic(int /*sq*/, int /*m*/, int /*bishop*/)
{
  //std::array<uint64_t, 4096> blockers{0};
  //std::array<uint64_t, 4096> attacked{0}; // Attacked squares, accounting for blockers
  //uint64_t magic{0};
  //std::array<uint64_t, 4096> used{0};

#if 0
  for (int k = 0; k < 100000000; ++k)
  {
    magic = random_uint64_fewbits();
    if (count_1s((mask * magic) & 0xFF00000000000000ULL) < 6)
      continue;
    for (int i = 0; i < 4096; ++i)
    {
      used[i] = 0ULL;
    }

    int fail{0};
    for (int i = 0; !fail && i < (1 << n); ++i)
    {
      int key = m agic_hash_fn(blockers[i], magic, m);
      if (used[key] == 0ULL)
        used[key] = attacked[i];
      else if (used[key] != attacked[i])
        fail = 1;
    }
    if (!fail)
      return magic;
  }
#endif
  //printf("***Failed***\n");
  return 0ULL;
}

uint64_t Move_generator::perft(int depth, Board& board, std::atomic_flag& is_cancelled)
{
  uint64_t nodes{0};

  if (depth == 0)
  {
    return uint64_t{1};
  }

  auto color = board.get_active_color();
  auto moves = Move_generator::generate_pseudo_legal_moves(board);
  for (auto m : moves)
  {
    auto tmp_board = Board{board};
    [[maybe_unused]] auto succeeded = tmp_board.move_no_verify(m);
    MY_ASSERT(succeeded, "Invalid move");

    if (!tmp_board.is_in_check(color))
    {
      nodes += perft(depth - 1, tmp_board, is_cancelled);
    }
    if (is_cancelled.test())
    {
      return nodes;
    }
  }

  return nodes;
}
