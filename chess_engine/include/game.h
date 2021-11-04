#ifndef GAME_H
#define GAME_H

class Player;

/**
 * A game is the overarching container for the random elements of a game.
 */
class Game
{
public:
  Game();

  ~Game();

  /**
   * @returns the next player whose turn it is
   */
  Player* get_next_player();

  bool make_move(Player const& player);

  /**
   * Runs a game of chess
   */
  void run();

  /**
   * Returns the opposite of the opposing player
   * @param player The player whose opponent to return
   * @return The opponent of the given player
   */
  Player& opponent_of(Player const& player);

private:
  std::unique_ptr<Player> m_player1{nullptr};
  std::unique_ptr<Player> m_player2{nullptr};
  Player* m_currentPlayer{nullptr};
};
#endif
