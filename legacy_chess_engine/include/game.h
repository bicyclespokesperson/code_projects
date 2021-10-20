#ifndef GAME_H
#define GAME_H

class Player;

/**
 * A game is the overarching container for the random elements of a game.
 */
class Game
{

public:
  /**
   * @returns the next player whose turn it is
   */
  static Player& get_next_player();

  /**
   * Sets up the board by putting the pieces in place.
   */
  static void initialize();

  /**
   * Returns the opposite of the opposing player
   * @param player The player whose opponent to return
   * @return The opponent of the given player
   */
  static Player& opponent_of(Player const& player);

  ~Game();

private:
  Game();
  static inline Player* _player1{nullptr};
  static inline Player* _player2{nullptr};
  static inline Player* _currentPlayer{nullptr};
};
#endif
