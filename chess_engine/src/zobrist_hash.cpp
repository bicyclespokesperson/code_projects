#include "zobrist_hash.h"
#include "board.h"

// https://www.chessprogramming.org/Zobrist_Hashing

namespace
{

constexpr int c_piece_count{6};
constexpr int c_pieces_region_size{c_board_dimension_squared * c_piece_count};
constexpr int c_black_pieces_offset{0};
constexpr int c_white_pieces_offset{c_black_pieces_offset + c_pieces_region_size};

constexpr int c_black_to_move_offset{c_white_pieces_offset + c_pieces_region_size};
constexpr int c_black_to_move_region_size{1};

constexpr int c_castling_rights_offset{c_black_to_move_offset + c_black_to_move_region_size};
constexpr int c_castling_rights_region_size{16};

constexpr int c_en_passant_file_offset{c_castling_rights_offset + c_castling_rights_region_size};
[[maybe_unused]] constexpr int c_en_passant_region_size{8};

} // namespace

const auto Zobrist_hash::m_random_numbers = []
{
  //NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init) This function will initialize the array
  std::array<zhash_t, 793> result;

  // Initialize with constant seed for run to run repeatability
  std::seed_seq seq{267403289, -1616180330, -870258931, -648938664, -1040157715, -195351127, -728458912, 1033321512};
  std::mt19937 eng(seq);
  std::uniform_int_distribution<int> dist{};

  std::generate(result.begin(), result.end(),
                [&]
                {
                  return dist(eng);
                });

  MY_ASSERT(c_en_passant_file_offset + c_en_passant_region_size == result.size(), "Random number count is wrong");

  return result;
}();

Zobrist_hash::Zobrist_hash(Board const& board)
{
  constexpr static std::array piece_types{Piece::pawn, Piece::knight, Piece::bishop,
                                          Piece::rook, Piece::queen,  Piece::king};
  for (auto piece : piece_types)
  {
    auto const piece_offset = static_cast<uint8_t>(piece) - static_cast<uint8_t>(Piece::pawn);
    for (auto const sq_index : board.get_piece_set(Color::black, piece))
    {
      auto const rand_index = c_black_pieces_offset + (c_board_dimension_squared * piece_offset) + sq_index;
      m_hash ^= m_random_numbers[rand_index];
    }
    for (auto const sq_index : board.get_piece_set(Color::white, piece))
    {
      auto const rand_index = c_white_pieces_offset + (c_board_dimension_squared * piece_offset) + sq_index;
      m_hash ^= m_random_numbers[rand_index];
    }
  }

  if (board.get_active_color() == Color::black)
  {
    update_player_to_move();
  }

  update_en_passant_square(board.get_en_passant_square());
  update_castling_rights(board.get_castling_rights());
}

void Zobrist_hash::update_en_passant_square(Bitboard ep_square)
{
  if (!ep_square.is_empty())
  {
    m_hash ^= m_random_numbers[c_en_passant_file_offset + Coordinates{ep_square.bitscan_forward()}.y()];
  }
}

void Zobrist_hash::update_castling_rights(Castling_rights rights)
{
  // Castling rights is packed into a uint8, so reinterpret casting to uint will give us a value 0 <= val < 16
  static_assert(sizeof(Castling_rights) == 1);

  //NOLINTNEXTLINE
  auto const as_uint8 = 0x0f & *reinterpret_cast<uint8_t*>(&rights);
  MY_ASSERT(as_uint8 < c_castling_rights_region_size, "Value is not a valid castling rights object");

  m_hash ^= m_random_numbers[c_castling_rights_offset + as_uint8];
}

void Zobrist_hash::update_piece_location(Color color, Piece piece, Coordinates location)
{
  // This array can be indexed by color
  constexpr static std::array piece_offsets{c_black_pieces_offset, c_white_pieces_offset};

  auto const piece_offset = static_cast<uint8_t>(piece) - static_cast<uint8_t>(Piece::pawn);
  auto const rand_index_base = piece_offsets[static_cast<uint8_t>(color)] + (c_board_dimension_squared * piece_offset);
  m_hash ^= m_random_numbers[rand_index_base + location.square_index()];
}

void Zobrist_hash::update_player_to_move()
{
  m_hash ^= m_random_numbers[c_black_to_move_offset];
}

zhash_t Zobrist_hash::get_hash() const
{
  return m_hash;
}

std::ostream& operator<<(std::ostream& os, Zobrist_hash const& self)
{
  os << std::to_string(self.get_hash());
  return os;
}
