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

bool is_drawn(Board const& board, Threefold_repetition_detector const& detector)
{
  auto color = board.get_active_color();
  if (board.is_in_stalemate(color))
  {
    return true;
  }

  if (!(board.has_sufficient_material(color) && board.has_sufficient_material(opposite_color(color))))
  {
    return true;
  }

  if (board.get_halfmove_clock() >= 50)
  {
    return true;
  }

  return detector.is_drawn();
}

void Game::play_game(Player& white_player, Player& black_player)
{
  Board board;
  Threefold_repetition_detector detector;
  std::vector<std::string> move_list;
  bool white_to_move{true};
  Game_state state{Game_state::in_progress};

  std::cout << board;
  while (state == Game_state::in_progress)
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
        detector.add_fen(board.to_fen());

        std::cout << "Board state after " << move_list.size() << " half moves\n";
        std::cout << board;
        auto const color = board.get_active_color();
        if (board.is_in_checkmate(color))
        {
          state = (white_to_move) ? Game_state::black_victory : Game_state::white_victory;
        }

        if (is_drawn(board, detector))
        {
          state = Game_state::draw;
        }
      }
      else
      {
        std::cout << "\n ------------ Invalid Move ------------ \n\n";
      }
    }
    else
    {
      state = (white_to_move) ? Game_state::black_victory : Game_state::white_victory;
    }
  }

  if (state == Game_state::draw)
  {
    std::cout << "Game is a draw\n";
  }
  else
  {
    std::string winning_player = ((state == Game_state::black_victory) ? std::string{"Black"} : std::string{"White"});
    std::cout << winning_player << " is victorious!\n";
  }

  std::cout << "Moves: ";
  for (auto const& move : move_list)
  {
    std::cout << move << " ";
  }
  std::cout << "\n";
}

