#ifndef PIECE_H
#define PIECE_H

class Square;
class Player;

enum class Color
{
  black = 0,
  white
};

/**
 * The superclass for all the Chess pieces
 */
class Piece
{

public:
  /**
   * Constructs a new piece.
   * @param owner The owner of the piece
   * @param _color The color of the piece
   * @param location The location of the piece
   */
  Piece(Player& owner, Color color, Square const& location);

  /**
   * The destructor of the piece.
   */
  virtual ~Piece();

  /**
   * @return True if the piece's color is white.
   */
  bool is_white() const;

  /**
   * Moves the piece to the specified square
   * @param by_player The player who moved the piece
   * @param to The destination square
   * @return True if the piece captured another piece
   */
  virtual bool move_to(Player& by_player, Square const& to);

  /**
   * @return the value of the piece
   */
  virtual int value() const = 0;

  /**
   * @return The color of the piece
   */
  Color color() const;

  /**
   * Prints the piece
   * @param out_stream The outstream to print to
   */
  virtual void display(std::ostream& out_stream) const = 0;

  /**
   * Checks if a piece can legally move to a square
   * @param location The square to move to
   * @return True if the move is legal
   */
  virtual bool can_move_to(Square const& location) const = 0;

  /**
   * Changes the piece's location
   * @param square The new location
   */
  virtual void set_location(Square const& square);

  /**
   * @return True if the piece is on a square
   */
  bool is_on_square() const;

  /**
   * @return The location of the piece
   */
  Square const& location() const;

  /**
   * @return the piece's owner
   */
  Player& owner();

  /**
   * Sets the piece's owner to the specified piece
   * @param new_owner The piece's new owner
   */
  void set_owner(Player& new_owner);

private:
  Color _color{Color::black};
  Square const* _location{nullptr};
  Player* _owner{nullptr};
};
#endif
