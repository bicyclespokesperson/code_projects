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

  void player_vs_player();

  void player_vs_computer(Color player_color);

  void computer_vs_computer();

  void play_game(Player& white_player, Player& black_player);
private:
};
#endif
