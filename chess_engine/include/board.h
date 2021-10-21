#ifndef BOARD_H
#define BOARD_H

#include <coordinates.h>
#include <square.h>

class Board
{
public:

  Board();

  ~Board();

  /**
   * Returns the square at the xy location
   * @param x The column of the desired square (A letter)
   * @param y The row coordinate of the desired square
   * @return The square at the desired location
   */
  Square const& square_at(Coordinates coords) const;

  Square& square_at(Coordinates coords);

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
  void display(std::ostream& out_stream) const;

  /**
   * Initializes the board with pieces in their standard positions
   */
  void setup();

  /**
   * Return false on invalid move
   */
  bool make_move(Coordinates from, Coordinates to);
private:

  /**
   * Ensure that all the coordinates in black and white pieces point to valid
   * pieces on the board
   */
  bool validate_() const;

  constexpr static size_t c_board_size{64};
  constexpr static size_t c_initial_piece_count{20};

  std::array<Square, c_board_size> m_squares{};

  //TODO(jere9309): Should these be arrays? Would that improve copying performance?
  std::vector<Coordinates> m_black_pieces{};
  std::vector<Coordinates> m_white_pieces{};
};
#endif
