#include "game.h"
#include "board.h"
#include "player.h"
#include "square.h"

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

  board.display(std::cout);

  bool game_in_progress = true;
  auto* player = get_next_player();
  while (game_in_progress)
  {
    if (auto move = player->prompt_move(std::cin, std::cout))
    {
      if (board.make_move(move->first, move->second))
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
    board.display(std::cout);
  }
  board.display(std::cout);
}

#if 0
bool Game::make_move(Player const& player)
{
  bool valid_move = false;
  bool still_playing = true;

  // Keep trying to make a move until a valid one is requested, or until
  // the player resigns.
  while (!valid_move && still_playing)
  {
    auto move = player.prompt_move(std::cin, std::cout);

    if (!move)
    {
      still_playing = false;
    }
    else if (Board::get_board().square_at(move->first.get_x(), move->first.get_y()).occupied())
    {
      Piece* occupier = &Board::get_board().square_at(move->first.get_x(), move->first.get_y()).occupied_by();

      if (my_pieces().find(occupier) == my_pieces().end())
      {
        std::cout << "Please move one of your own pieces." << std::endl;
      }
      else
      {
        valid_move = occupier->move_to(*this, (move->second));
        if (!valid_move)
        {
          std::cout << "please enter a valid move for the piece, and ";
          std::cout << "ensure that it does not leave the King in check." << std::endl;
        }
      }
    }
    else
    {
      std::cout << "please choose a square with a piece on it." << std::endl;
    }
  }

  return still_playing;
}
#endif

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
