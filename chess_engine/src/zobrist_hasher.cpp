#include "zobrist_hasher.h"
#include "board.h"

// https://www.chessprogramming.org/Zobrist_Hashing

namespace {
  
constexpr int c_pieces_region_size{c_board_dimension_squared * (static_cast<uint8_t>(Piece::_count) - static_cast<uint8_t>(Piece::pawn))};
constexpr int c_black_to_move_offset{c_pieces_region_size};
constexpr int c_black_to_move_region_size{1};

constexpr int c_castling_rights_offset{c_black_to_move_offset + c_black_to_move_region_size};
constexpr int c_castling_rights_region_size{16};

constexpr int c_en_passant_file_offset{c_castling_rights_offset + c_castling_rights_region_size};
[[maybe_unused]] constexpr int c_en_passant_region_size{8};

} // namespace

Zobrist_hasher::Zobrist_hasher()
{
  // Initialize with constant seed for run to run repeatability
  std::seed_seq seq{267403289, -1616180330, -870258931, -648938664, -1040157715, -195351127, -728458912, 1033321512};
  std::mt19937 eng(seq);
  std::uniform_int_distribution<int> dist{};

  std::generate(m_random_numbers.begin(), m_random_numbers.end(), [&]
                {
                  return dist(eng);
                });

  MY_ASSERT(c_en_passant_file_offset + c_en_passant_region_size == m_random_numbers.size(), "Random number count is wrong");
}

zhash_t Zobrist_hasher::hash_from_position(Board const& board) const
{
  zhash_t result{0};

  constexpr static std::array piece_types{Piece::pawn, Piece::knight, Piece::bishop, Piece::rook, Piece::queen, Piece::king};
  for (auto piece : piece_types)
  {
    auto const piece_offset = static_cast<uint8_t>(piece) - static_cast<uint8_t>(Piece::pawn);
    for (auto sq_index : board.get_piece_set(Color::black, piece))
    {
      auto const rand_index = c_board_dimension_squared * piece_offset + sq_index;
      result ^= m_random_numbers[rand_index];
    }
  }
  
  if (board.get_active_color() == Color::black)
  {
    result = update_player_to_move(result);
  }

  result = update_en_passant_square(board.get_en_passant_square(), result);
  result = update_castling_rights(board.get_castling_rights(), result);

  return result;
}

zhash_t Zobrist_hasher::update_en_passant_square(Bitboard ep_square, zhash_t zhash) const
{
  if (!ep_square.is_empty())
  {
    return zhash ^ m_random_numbers[c_en_passant_file_offset + Coordinates{ep_square.bitscan_forward()}.y()];
  }
  return zhash;
}

zhash_t Zobrist_hasher::update_castling_rights(Castling_rights rights, zhash_t zhash) const
{
  // Castling rights is packed into a uint8, so reinterpret casting to uint will give us a value 0 <= val < 16
  static_assert(sizeof(Castling_rights) == 1);

  //NOLINTNEXTLINE
  auto const as_uint8 = *reinterpret_cast<uint8_t*>(&rights);
  MY_ASSERT(as_uint8 < c_castling_rights_region_size, "Value is not a valid castling rights object");

  return zhash ^ m_random_numbers[c_castling_rights_offset + as_uint8];
}

zhash_t Zobrist_hasher::update_player_to_move(zhash_t zhash) const
{
  return zhash ^ m_random_numbers[c_black_to_move_offset];
}

zhash_t Zobrist_hasher::update_with_move(Piece piece, Move m, zhash_t zhash) const
{
  auto const piece_offset = static_cast<uint8_t>(piece) - static_cast<uint8_t>(Piece::pawn);
  zhash ^= m_random_numbers[c_board_dimension_squared * piece_offset + m.to.square_index()];
  zhash ^= m_random_numbers[c_board_dimension_squared * piece_offset + m.from.square_index()];
  return zhash;
}

