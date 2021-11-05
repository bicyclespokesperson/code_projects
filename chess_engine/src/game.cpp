#include "game.h"
#include "board.h"
#include "player.h"

Game::Game() = default;

Game::~Game() = default;

/**
 * Initialize the board by putting the pieces in the correct locations
 */
void Game::run()
{
  m_player1 = std::make_unique<Player>("White player");
  m_player2 = std::make_unique<Player>("Black player");

  Board board;

  std::cout << board;

  bool game_in_progress = true;
  auto* player = get_next_player();
  while (game_in_progress)
  {
    if (auto move = player->prompt_move(std::cin, std::cout))
    {
      // if (board.try_move_uci(*move))
      if (board.try_move_uci(*move))
      {
        player = get_next_player();
      }
      else
      {
        std::cout << "\n --------- That move is invalid --------- \n";
      }
    }
    else
    {
      game_in_progress = false;
    }

    std::cout << board;
    auto const color = board.get_active_color();
    if (board.is_in_checkmate(color))
    {
      game_in_progress = false;
      std::cout << (color == Color::white ? "black" : "white") << " is victorious!" << std::endl;
    }
  }
}

Player* Game::get_next_player()
{
  if (!m_currentPlayer)
  {
    // Return player1 the first time this is called
    m_currentPlayer = m_player1.get();
  }
  else
  {
    m_currentPlayer = &opponent_of(*m_currentPlayer);
  }

  return m_currentPlayer;
}

Player& Game::opponent_of(Player const& player)
{
  Player* result = m_player1.get();

  // Make sure to return the opposite of the player who was passed in.
  if (&player == m_player1.get())
  {
    result = m_player2.get();
  }

  return *result;
}
