#include "move_generator.h"
#include "board.h"
#include "my_assert.h"

namespace
{

constexpr void update_if_in_bounds_(Bitboard& bb, int x, int y)
{
  if (0 <= x && x < c_board_dimension && 0 <= y && y < c_board_dimension)
  {
    bb.set_square(Coordinates{x, y}.square_index());
  }
}

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

// This array can be indexed by Color, so the operator to be used for black pawns is first
constexpr std::array c_promotion_ranks{Bitboard_constants::first_rank, Bitboard_constants::eighth_rank};
constexpr Bitboard get_pawn_promotion_rank(Color color)
{
  return c_promotion_ranks[static_cast<int32_t>(color)];
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

} // namespace

std::array<std::array<Bitboard, Compass_dir::_count>, c_board_dimension_squared> Move_generator::m_ray_attacks = []
{
  return Move_generator::initialize_ray_attacks_();
}();

std::array<Bitboard, c_board_dimension_squared> Move_generator::m_knight_attacks = []
{
  return Move_generator::initialize_knight_attacks_();
}();

std::array<Bitboard, c_board_dimension_squared> Move_generator::m_king_attacks = []
{
  return Move_generator::initialize_king_attacks_();
}();

std::array<std::array<Bitboard, Compass_dir::_count>, c_board_dimension_squared>
Move_generator::initialize_ray_attacks_()
{
  std::array<std::array<Bitboard, Compass_dir::_count>, c_board_dimension_squared> result;
  for (int8_t x{0}; x < c_board_dimension; ++x)
  {
    for (int8_t y{0}; y < c_board_dimension; ++y)
    {
      for (Compass_dir dir = Compass_dir::north; dir < Compass_dir::_count; ++dir)
      {
        auto square_index = Coordinates{x, y}.square_index();
        Bitboard& bb = result[square_index][dir];

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
  return result;
} // initialize_ray_attacks

std::array<Bitboard, c_board_dimension_squared> Move_generator::initialize_knight_attacks_()
{
  std::array<Bitboard, c_board_dimension_squared> result;
  for (int8_t x{0}; x < c_board_dimension; ++x)
  {
    for (int8_t y{0}; y < c_board_dimension; ++y)
    {
      auto square_index = Coordinates{x, y}.square_index();
      auto& bb = result[square_index];

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
  return result;
}

std::array<Bitboard, c_board_dimension_squared> Move_generator::initialize_king_attacks_()
{
  std::array<Bitboard, c_board_dimension_squared> result;
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

  return result;
}

Bitboard Move_generator::get_positive_ray_attacks_(Coordinates square, Compass_dir dir, Bitboard occupied)
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

Bitboard Move_generator::get_negative_ray_attacks_(Coordinates square, Compass_dir dir, Bitboard occupied)
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
  return m_knight_attacks[square.square_index()];
}

Bitboard Move_generator::king_attacks(Coordinates square, Bitboard /* occupied */)
{
  return m_king_attacks[square.square_index()];
}

Bitboard Move_generator::pawn_short_advances(Color color, Bitboard pawns, Bitboard occupied)
{
  auto const bitshift_fn = get_pawn_shift_fn(color);
  auto const one_space_moves = (pawns.*bitshift_fn)(c_board_dimension) & ~occupied;
  auto const promotion_rank = get_pawn_promotion_rank(color); // Handle pawn promotions separately
  return one_space_moves & ~promotion_rank;
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

Bitboard Move_generator::pawn_promotions(Color color, Bitboard pawns, Bitboard occupied)
{
  auto const bitshift_fn = get_pawn_shift_fn(color);
  auto const one_space_moves = (pawns.*bitshift_fn)(c_board_dimension) & ~occupied;
  auto const promotion_rank = get_pawn_promotion_rank(color); // Handle pawn promotions separately
  return one_space_moves & promotion_rank;
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

Bitboard Move_generator::pawn_east_attacks_no_promote(Color color, Bitboard pawns, Bitboard enemies)
{
  auto const bitshift_fn = get_pawn_shift_fn(color);
  auto const promotion_rank = get_pawn_promotion_rank(color); // Handle pawn promotions separately
  auto const shift_distance = get_east_shift_distance(color); // new
  auto const east_attacks = (pawns.*bitshift_fn)(shift_distance) & ~Bitboard_constants::a_file & enemies;
  return east_attacks & ~promotion_rank;
}

Bitboard Move_generator::pawn_east_attacks_with_promote(Color color, Bitboard pawns, Bitboard enemies)
{
  auto const bitshift_fn = get_pawn_shift_fn(color);
  auto const promotion_rank = get_pawn_promotion_rank(color); // Handle pawn promotions separately
  auto const shift_distance = get_east_shift_distance(color); // new
  auto const east_attacks = (pawns.*bitshift_fn)(shift_distance) & ~Bitboard_constants::a_file & enemies;
  return east_attacks & promotion_rank;
}

Bitboard Move_generator::pawn_west_attacks_no_promote(Color color, Bitboard pawns, Bitboard enemies)
{
  auto const bitshift_fn = get_pawn_shift_fn(color);
  auto const shift_distance = get_west_shift_distance(color); // new
  auto const promotion_rank = get_pawn_promotion_rank(color); // Handle pawn promotions separately
  auto const west_attacks = (pawns.*bitshift_fn)(shift_distance) & ~Bitboard_constants::h_file & enemies;
  return west_attacks & ~promotion_rank;
}

Bitboard Move_generator::pawn_west_attacks_with_promote(Color color, Bitboard pawns, Bitboard enemies)
{
  auto const bitshift_fn = get_pawn_shift_fn(color);
  auto const shift_distance = get_west_shift_distance(color);
  auto const promotion_rank = get_pawn_promotion_rank(color); // Handle pawn promotions separately
  auto const west_attacks = (pawns.*bitshift_fn)(shift_distance) & ~Bitboard_constants::h_file & enemies;
  return west_attacks & promotion_rank;
}

void Move_generator::generate_castling_moves(Board const& board, Color color, std::vector<Move>& moves)
{
  static constexpr Move short_castle_white{{4, 0}, {6, 0}};
  static constexpr Move long_castle_white{{4, 0}, {2, 0}};
  static constexpr Move short_castle_black{{4, 7}, {6, 7}};
  static constexpr Move long_castle_black{{4, 7}, {2, 7}};

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
      auto attacks = piece_move_functions[i](Coordinates{piece_location}, board.get_occupied_squares());
      attacks &= ~board.get_all(color); // Throw out any moves to a square that is already occupied by our color
      for (auto end_location : attacks)
      {
        moves.emplace_back(Coordinates{piece_location}, Coordinates{end_location});
      }
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
    moves.emplace_back(Coordinates{location + offset_from_end_square}, Coordinates{location});
    MY_ASSERT(moves.back().to.y() != 0 && moves.back().to.y() != 7, "Promotions should be handled separately");
  }

  // Handle long advances
  for (auto location : pawn_long_advances(color, board.get_piece_set(color, Piece::pawn), board.get_occupied_squares()))
  {
    moves.emplace_back(Coordinates{location + 2 * offset_from_end_square}, Coordinates{location});
  }

  // Handle east captures
  auto east_offset = get_east_capture_offset(color);
  for (auto location : pawn_east_attacks_no_promote(color, board.get_piece_set(color, Piece::pawn),
                                                    board.get_all(opposite_color(color))))
  {
    moves.emplace_back(Coordinates{location + east_offset}, Coordinates{location});
  }

  // Handle promotion / east capture moves
  for (auto location : pawn_east_attacks_with_promote(color, board.get_piece_set(color, Piece::pawn),
                                                      board.get_all(opposite_color(color))))
  {
    Coordinates from{location + east_offset};
    Coordinates to{location};
    moves.emplace_back(from, to, Move_type::normal, Piece::bishop);
    moves.emplace_back(from, to, Move_type::normal, Piece::knight);
    moves.emplace_back(from, to, Move_type::normal, Piece::rook);
    moves.emplace_back(from, to, Move_type::normal, Piece::queen);
  }

  // Handle west captures
  auto west_offset = get_west_capture_offset(color);
  for (auto location : pawn_west_attacks_no_promote(color, board.get_piece_set(color, Piece::pawn),
                                                    board.get_all(opposite_color(color))))
  {
    moves.emplace_back(Coordinates{location + west_offset}, Coordinates{location});
  }

  // Handle promotion / west capture moves
  for (auto location : pawn_west_attacks_with_promote(color, board.get_piece_set(color, Piece::pawn),
                                                      board.get_all(opposite_color(color))))
  {
    Coordinates from{location + west_offset};
    Coordinates to{location};
    moves.emplace_back(from, to, Move_type::normal, Piece::bishop);
    moves.emplace_back(from, to, Move_type::normal, Piece::knight);
    moves.emplace_back(from, to, Move_type::normal, Piece::rook);
    moves.emplace_back(from, to, Move_type::normal, Piece::queen);
  }

  // Handle promotions
  for (auto location : pawn_promotions(color, board.get_piece_set(color, Piece::pawn), board.get_occupied_squares()))
  {
    Coordinates from{location + offset_from_end_square};
    Coordinates to{location};
    moves.emplace_back(from, to, Move_type::normal, Piece::bishop);
    moves.emplace_back(from, to, Move_type::normal, Piece::knight);
    moves.emplace_back(from, to, Move_type::normal, Piece::rook);
    moves.emplace_back(from, to, Move_type::normal, Piece::queen);
  }

  // Handle en passant
  for (auto location :
       pawn_east_attacks_no_promote(color, board.get_piece_set(color, Piece::pawn), board.get_en_passant_square()))
  {
    moves.emplace_back(Coordinates{location + east_offset}, Coordinates{location}, Move_type::en_passant);
  }

  for (auto location :
       pawn_west_attacks_no_promote(color, board.get_piece_set(color, Piece::pawn), board.get_en_passant_square()))
  {
    moves.emplace_back(Coordinates{location + west_offset}, Coordinates{location}, Move_type::en_passant);
  }
}

std::vector<Move> Move_generator::generate_pseudo_legal_moves(Board const& board, Color color)
{
  std::vector<Move> pseudo_legal_moves;
  pseudo_legal_moves.reserve(218); // Max possible number of chess moves in a position

  generate_pawn_moves(board, color, pseudo_legal_moves);
  generate_piece_moves(board, color, pseudo_legal_moves);
  generate_castling_moves(board, color, pseudo_legal_moves);

  return pseudo_legal_moves;
}

std::vector<Move> Move_generator::generate_legal_moves(Board const& board, Color color)
{
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

uint64_t perft(int depth, Board& board, std::atomic_flag& is_cancelled)
{
  uint64_t nodes{0};

  if (depth == 0)
  {
    return uint64_t{1};
  }

  auto color = board.get_active_color();
  auto moves = Move_generator::generate_pseudo_legal_moves(board, color);
  for (auto m : moves)
  {
    auto tmpBoard = Board{board};
    [[maybe_unused]] auto captured_piece = tmpBoard.move_no_verify(m);		
    MY_ASSERT(captured_piece.has_value(), "Invalid move");

    if (!tmpBoard.is_in_check(color))
    {
      nodes += perft(depth - 1, tmpBoard, is_cancelled);
    }
  }

  return nodes;
}
