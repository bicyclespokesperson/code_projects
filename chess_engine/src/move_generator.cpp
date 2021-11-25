#include "move_generator.h"
#include "board.h"
#include "my_assert.h"

namespace
{

std::array<int, 64> RBits = {12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10,
                 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10,
                 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12};

std::array<int, 64> BBits = {6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5,
                 5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6};

// Generated with code from: https://www.chessprogramming.org/Looking_for_Magics
constexpr std::array<uint64_t, 64> c_rook_magics{
  0x2080020500400f0ULL,  0x28444000400010ULL,   0x20000a1004100014ULL, 0x20010c090202006ULL,  0x8408008200810004ULL,
  0x1746000808002ULL,    0x2200098000808201ULL, 0x12c0002080200041ULL, 0x104000208e480804ULL, 0x8084014008281008ULL,
  0x4200810910500410ULL, 0x100014481c20400cULL, 0x4014a4040020808ULL,  0x401002001010a4ULL,   0x202000500010001ULL,
  0x8112808005810081ULL, 0x40902108802020ULL,   0x42002101008101ULL,   0x459442200810c202ULL, 0x81001103309808ULL,
  0x8110000080102ULL,    0x8812806008080404ULL, 0x104020000800101ULL,  0x40a1048000028201ULL, 0x4100ba0000004081ULL,
  0x44803a4003400109ULL, 0xa010a00000030443ULL, 0x91021a000100409ULL,  0x4201e8040880a012ULL, 0x22a000440201802ULL,
  0x30890a72000204ULL,   0x10411402a0c482ULL,   0x40004841102088ULL,   0x40230000100040ULL,   0x40100010000a0488ULL,
  0x1410100200050844ULL, 0x100090808508411ULL,  0x1410040024001142ULL, 0x8840018001214002ULL, 0x410201000098001ULL,
  0x8400802120088848ULL, 0x2060080000021004ULL, 0x82101002000d0022ULL, 0x1001101001008241ULL, 0x9040411808040102ULL,
  0x600800480009042ULL,  0x1a020000040205ULL,   0x4200404040505199ULL, 0x2020081040080080ULL, 0x40a3002000544108ULL,
  0x4501100800148402ULL, 0x81440280100224ULL,   0x88008000000804ULL,   0x8084060000002812ULL, 0x1840201000108312ULL,
  0x5080202000000141ULL, 0x1042a180880281ULL,   0x900802900c01040ULL,  0x8205104104120ULL,    0x9004220000440aULL,
  0x8029510200708ULL,    0x8008440100404241ULL, 0x2420001111000bdULL,  0x4000882304000041ULL,
};

constexpr std::array<uint64_t, 64> c_bishop_magics{
  0x100420000431024ULL,  0x280800101073404ULL,  0x42000a00840802ULL,   0xca800c0410c2ULL,     0x81004290941c20ULL,
  0x400200450020250ULL,  0x444a019204022084ULL, 0x88610802202109aULL,  0x11210a0800086008ULL, 0x400a08c08802801ULL,
  0x1301a0500111c808ULL, 0x1280100480180404ULL, 0x720009020028445ULL,  0x91880a9000010a01ULL, 0x31200940150802b2ULL,
  0x5119080c20000602ULL, 0x242400a002448023ULL, 0x4819006001200008ULL, 0x222c10400020090ULL,  0x302008420409004ULL,
  0x504200070009045ULL,  0x210071240c02046ULL,  0x1182219000022611ULL, 0x400c50000005801ULL,  0x4004010000113100ULL,
  0x2008121604819400ULL, 0xc4a4010000290101ULL, 0x404a000888004802ULL, 0x8820c004105010ULL,   0x28280100908300ULL,
  0x4c013189c0320a80ULL, 0x42008080042080ULL,   0x90803000c080840ULL,  0x2180001028220ULL,    0x1084002a040036ULL,
  0x212009200401ULL,     0x128110040c84a84ULL,  0x81488020022802ULL,   0x8c0014100181ULL,     0x2222013020082ULL,
  0xa00100002382c03ULL,  0x1000280001005c02ULL, 0x84801010000114cULL,  0x480410048000084ULL,  0x21204420080020aULL,
  0x2020010000424a10ULL, 0x240041021d500141ULL, 0x420844000280214ULL,  0x29084a280042108ULL,  0x84102a8080a20a49ULL,
  0x104204908010212ULL,  0x40a20280081860c1ULL, 0x3044000200121004ULL, 0x1001008807081122ULL, 0x50066c000210811ULL,
  0xe3001240f8a106ULL,   0x940c0204030020d4ULL, 0x619204000210826aULL, 0x2010438002b00a2ULL,  0x884042004005802ULL,
  0xa90240000006404ULL,  0x500d082244010008ULL, 0x28190d00040014e0ULL, 0x825201600c082444ULL,
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


#if 1
int count_1s(uint64_t b)
{
  int r{0};
  for (; b; r++, b &= b - 1)
    ;
  return r;
}
#endif

const std::array<int, 64> BitTable {63, 30, 3,  32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,  51, 21, 43,
                          45, 10, 18, 47, 1,  54, 9,  57, 0,  35, 62, 31, 40, 4,  49, 5,  52, 26, 60, 6,  23, 44,
                          46, 27, 56, 16, 7,  39, 48, 24, 59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8};

int pop_1st_bit(uint64_t* bb)
{
  uint64_t b = *bb ^ (*bb - 1);
  auto fold = static_cast<uint32_t>((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return BitTable[(fold * 0x783a9b23) >> 26];
}

uint64_t index_to_uint64(int index, int bits, uint64_t m)
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
  return result;
}

int magic_hash_fn(uint64_t blockers, uint64_t magic, int bits)
{
  return static_cast<int>((blockers * magic) >> (64 - bits));
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
  for (int8_t index{0}; index < c_board_dimension_squared; ++index)
  {
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

#if 1 // I think this can be replaced by rook_attacked_squares, or vice versa
    Bitboard cannot_block{Bitboard_constants::all_outer_squares};
    Bitboard current_location;
    current_location.set_square(index);
    if (!(current_location & Bitboard_constants::a_file).is_empty())
    {
      cannot_block &= (~Bitboard_constants::a_file);
    }
    if (!(current_location & Bitboard_constants::h_file).is_empty())
    {
      cannot_block &= (~Bitboard_constants::h_file);
    }
    if (!(current_location & Bitboard_constants::first_rank).is_empty())
    {
      cannot_block &= (~Bitboard_constants::first_rank);
    }
    if (!(current_location & Bitboard_constants::eighth_rank).is_empty())
    {
      cannot_block &= (~Bitboard_constants::eighth_rank);
    }
    cannot_block |= Bitboard_constants::corners;

    // Add rook directions to rook table
    constexpr static std::array rook_directions{Compass_dir::north, Compass_dir::south, Compass_dir::west, Compass_dir::east};
    auto rook_mask = std::accumulate(rook_directions.cbegin(), rook_directions.cend(), Bitboard{0}, [&](Bitboard result_so_far, auto direction)
                                {
                                  return result_so_far | ray_attacks[index][direction];
                                });
    rook_magic_table[index] = Tables::Magic{rook_mask & ~cannot_block, c_rook_magics[index]};

    // Add bishop directions to bishop table
    constexpr static std::array bishop_directions{Compass_dir::northwest, Compass_dir::northeast, Compass_dir::southwest, Compass_dir::southeast};
    auto bishop_mask = std::accumulate(bishop_directions.cbegin(), bishop_directions.cend(), Bitboard{0}, [&](Bitboard result_so_far, auto direction)
                                {
                                  return result_so_far | ray_attacks[index][direction];
                                });
#endif
    
    // TODO: initialize rook and bishop attack table
    // https://stackoverflow.com/questions/67513005/how-to-generate-this-preinitialized-array-for-magic-bitboards
    // See Attacks::detail::_initBishopMagicTable in shallow blue source code
    auto const possible_blockers = bishop_mask & ~cannot_block;
    bishop_magic_table[index] = Tables::Magic{possible_blockers, c_bishop_magics[index]};


    bool bishop = true;
    Bitboard mask = bishop ? bishop_potential_blockers(index) : rook_potential_blockers(index);

    int n = count_1s(mask.val);
    //int n = mask.occupancy();
    MY_ASSERT(n == mask.occupancy(), "Occupancy mismatch");

    if (mask != possible_blockers) //, "Comparing to blocker generation methods");
    {
      std::cout << "Mine: \n" << possible_blockers << "\nTheirs\n" << mask;
      MY_ASSERT(false, "Blocker generation mismatch");
    }

    // Populate blockers and attackers table
    std::cout << "Value: " << n << "\n";
    for (int i = 0; i < (1 << n); ++i)
    {
      auto blockers = index_to_uint64(i, n, mask.val);
      auto key = magic_hash_fn(blockers, BBits[index], c_bishop_magics[index]);
      auto attacked_squares = bishop_attacked_squares(index, blockers);
      m_bishop_attacks[index][key] = attacked_squares; // Permutations?
      //attacked[i] = bishop ? bishop_attacked_squares(index, blockers[i]) : rook_attacked_squares(index, blockers[i]);
    }
  } // for each index
} // initialize_ray_attacks

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

#if 0
Bitboard bishop_attacks(Bitboard occ, Coordinates coords) {
   occ &= c_bishop_table[coords.square_index()].mask;
   occ *= c_bishop_table[coords.square_index()].magic;
   occ >>= (64-9);
   return m_bishop_attacks[coords.square_index()][occ.val];
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

  return gen_sliding_moves_(positive_directions, negative_directions, square, occupied);
}

Bitboard Move_generator::bishop_attacks(Coordinates square, Bitboard occupied)
{
  constexpr static std::array<Compass_dir, 2> positive_directions{Compass_dir::northeast, Compass_dir::northwest};
  constexpr static std::array<Compass_dir, 2> negative_directions{Compass_dir::southeast, Compass_dir::southwest};

  return gen_sliding_moves_(positive_directions, negative_directions, square, occupied);
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



uint64_t find_magic(int sq, int m, int bishop)
{
  std::array<uint64_t, 4096> blockers{0};
  std::array<uint64_t, 4096> attacked{0}; // Attacked squares, accounting for blockers 
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
      int key = magic_hash_fn(blockers[i], magic, m);
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

