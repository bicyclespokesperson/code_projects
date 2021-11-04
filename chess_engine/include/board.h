#ifndef BOARD_H
#define BOARD_H

#include "bitboard.h"
#include "coordinates.h"
#include "move.h"
#include "square.h"

class Board
{
public:
  static std::optional<Board> from_pgn(std::string_view pgn);

  static std::optional<Board> from_fen(std::string_view fen);

  Board();

  Board(Board const& other) = default;
  Board(Board&& other) = default;
  Board& operator=(Board const& other) = default;
  Board& operator=(Board& other) = default;

  ~Board() = default;

  std::string to_fen() const;

  /**
   * Checks if every square in between squares from and to are empty,
   * and if the squares are in a vertical line.
   * @param from The initial square
   * @param to The final square
   * @return True if there is a clear vertical line between the squares
   */
  bool is_clear_vertical(Coordinates from, Coordinates to) const;

  /**
   * Checks if every square in between squares from and to are empty,
   * and if the squares are in a horizontal line.
   * @param from The initial square
   * @param to The final square
   * @return True if there is a clear horizontal line between the squares
   */
  bool is_clear_horizontal(Coordinates from, Coordinates to) const;

  /**
   * Checks if every square in between squares from and to are empty,
   * and if the squares are in a diagonal line.
   * @param from The initial square
   * @param to The final square
   * @return True if there is a clear diagonal line between the squares
   */
  bool is_clear_diagonal(Coordinates from, Coordinates to) const;

  /**
   * Returns the distance between two squares on the board.
   * For example, A2 and A5 are three squares apart
   * Returns -1 if the squares are not in a line on the board.
   * @param from The first Square
   * @param to The Second Square
   * @return The distance between the two squares.
   */
  int distance_between(Coordinates from, Coordinates to) const;

  /**
   * Prints the square to the specified std::ostream
   * @param out_stream The stream to print to
   */

  /**
   * Initializes the board with pieces in their standard positions
   */
  void setup();

  /**
   * Return false on invalid move
   */
  bool try_move(Move m);

  /**
   * Attempt to make a move encoded in uci format ("e2 e4")
   * @return False if the move was a malformed move string, or illegal
   */
  bool try_move_uci(std::string_view move_str);

  /**
   * Attempt to make a move encoded in algebraic notation ("Bxc3")
   * @return False if the move was a malformed move string, or illegal
   */
  bool try_move_algebraic(std::string_view move_str);

  std::optional<Move> previous_move() const;

  bool check_castling_rights(Coordinates dest) const;

  std::vector<Coordinates> find_pieces_that_can_move_to(Piece piece, Color color, Coordinates target_square) const;

  Color current_turn_color() const;

  bool is_in_checkmate(Color color) const;

  Bitboard get_piece_set(Color color, Piece piece) const;

  Bitboard get_all(Piece piece) const;

  Bitboard get_all(Color color) const;

  Bitboard get_en_passant_square() const;

  Bitboard get_occupied_squares() const;

  bool is_occupied(Coordinates square) const;

  Color get_piece_color(Coordinates square) const;

  Piece get_piece(Coordinates square) const;

private:
  /**
   * Private constructor that doesn't initialize pieces
   */
  Board(int);

  /**
   * Ensure that all the coordinates in black and white pieces point to valid
   * pieces on the board
   */
  bool validate_() const;

  void update_king_locations_(Coordinates dest);
  void update_castling_rights_(Coordinates dest);
  bool update_castling_rights_fen_(char c);
  std::string castling_rights_to_fen_() const;

  bool is_in_check_(Color color) const;

  std::optional<Move> move_from_algebraic_(std::string_view move_param, Color color);
  std::optional<Move> move_from_uci_(std::string move_str);

  std::optional<std::pair<Coordinates, Piece>> perform_move_(Move m, Coordinates capture_location);
  void unperform_move_(Move m, std::optional<std::pair<Coordinates, Piece>> captured_piece);

  static void add_piece_(Color color, Piece piece, Coordinates to_add);
  static void remove_piece_(Color color, Piece piece, Coordinates to_remove);

  static void display_piece_locations_(std::vector<Coordinates> const& pieces);

  Move find_castling_rook_move_(Coordinates king_destination);

  constexpr static size_t c_board_size{c_board_dimension_squared};
  constexpr static size_t c_initial_piece_count{20};

  std::optional<Move> m_previous_move;

  std::array<Bitboard, static_cast<size_t>(Piece::_count)> m_bitboards;
  Bitboard m_en_passant_square{0};

  bool m_white_can_short_castle : 1 {true};
  bool m_white_can_long_castle : 1 {true};
  bool m_black_can_short_castle : 1 {true};
  bool m_black_can_long_castle : 1 {true};
};

std::ostream& operator<<(std::ostream& out, Board const& self);

#endif
