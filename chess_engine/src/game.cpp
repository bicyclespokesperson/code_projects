#include "game.h"
#include "board.h"
#include "player.h"
#include "meneldor_engine.h"

void Game::player_vs_player()
{
  User_player white_player("White player");
  User_player black_player("Black player");
  play_game(white_player, black_player);
}

void Game::player_vs_computer(Color player_color)
{
  if (player_color == Color::white)
  {
    User_player white_player("White player");
    Engine_player black_player("Black engine");
    play_game(white_player, black_player);
  }
  else
  {
    Engine_player white_player("White engine");
    User_player black_player("Black player");
    play_game(white_player, black_player);
  }
}

void Game::computer_vs_computer()
{
  Engine_player white_player("White engine");
  Engine_player black_player("Black engine");
  play_game(white_player, black_player);
}

void Game::play_game(Player& white_player, Player& black_player)
{
  Board board;
  std::vector<std::string> move_list;
  bool white_to_move{true};
  bool game_in_progress{true};

  std::cout << board;
  while (game_in_progress)
  {
    std::optional<std::string> move;
    if (white_to_move)
    {
      move = white_player.get_next_move(std::cin, std::cout);
    }
    else
    {
      move = black_player.get_next_move(std::cin, std::cout);
    }

    if (move)
    {
      if (auto result = board.try_move_uci(*move))
      {
        white_player.notify(*move);
        black_player.notify(*move);
        move_list.push_back(*move);
        white_to_move = !white_to_move;

        //TODO: Handle draws

        std::cout << "Board state after " << move_list.size() << " half moves\n";
        std::cout << board;
        auto const color = board.get_active_color();
        if (board.is_in_checkmate(color))
        {
          game_in_progress = false;

        }
      }
      else
      {
        std::cout << "\n ------------ Invalid Move ------------ \n\n";
      }
    }
    else
    {
      game_in_progress = false;
    }
  }

  std::string winning_player = (white_to_move ? std::string{"Black"} : std::string{"White"});
  std::cout << winning_player << " is victorious!\n";

  std::cout << "Moves: ";
  for (auto const& move : move_list)
  {
    std::cout << move << " ";
  }
  std::cout << "\n";
}

