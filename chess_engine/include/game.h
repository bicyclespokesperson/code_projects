#ifndef GAME_H
#define GAME_H

#include "chess_types.h"

class Player;

/**
 * Allows the user to play a game of chess
 */
class Game
{
public:
  Game() = default;

  ~Game() = default;

  Game(Game const&) = delete;
  Game(Game&&) = delete;

  Game& operator=(Game const&) = delete;
  Game& operator=(Game&&) = delete;

  /**
   * @returns the next player whose turn it is
   */
  Player* get_next_player();

  bool make_move(Player const& player);

  /**
   * Runs a game of chess
   */
  void player_vs_player();

  void player_vs_computer(Color player_color);

  void computer_vs_computer();

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
