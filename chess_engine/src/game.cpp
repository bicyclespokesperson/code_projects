#include "game.h"
#include "board.h"
#include "player.h"
#include "meneldor_engine.h"

/**
 * Initialize the board by putting the pieces in the correct locations
 */
void Game::player_vs_player()
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
      //if (board.try_move_uci(*move))
      if (board.try_move_algebraic(*move))
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
      std::string winning_player = (color == Color::white ? std::string{"black"} : std::string{"white"});
      std::cout << winning_player << " is victorious!" << std::endl;
    }
  }
}

void Game::player_vs_computer(Color player_color)
{
  m_player1 = std::make_unique<Player>((player_color == Color::white) ? "White player" : "Black player");

  Meneldor_engine engine;
  engine.initialize();

  Board board;

  std::cout << board;


  auto move_to_string = [](Move m)
  {
    std::stringstream ss;
    ss << m;
    return ss.str();
  };

  std::vector<std::string> move_list;
  bool game_in_progress = true;
  bool player_turn{player_color == Color::white};
  auto* player = get_next_player();
  while (game_in_progress)
  {
    if (player_turn)
    {
      if (auto move_str = player->prompt_move(std::cin, std::cout))
      {
        auto move = board.move_from_algebraic(*move_str, board.get_active_color());
        if (move && board.try_move(*move))
        {
          player_turn = !player_turn;
          engine.makeMove(move_to_string(*move));
          move_list.push_back(move_to_string(*move));
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
    }
    else
    {
      std::cout << "Engine thinking\n";

      senjo::GoParams params;
      auto const start = std::chrono::system_clock::now();
      auto const engine_move = engine.go(params, nullptr);
      auto const end = std::chrono::system_clock::now();
      std::chrono::duration<double> const elapsed_seconds = end - start;

      [[maybe_unused]] auto result = board.try_move_uci(engine_move);
      MY_ASSERT(result.has_value(), "Engine cannot play illegal move");
      engine.makeMove(engine_move);
      std::cout << "Engine played " << engine_move << " after thinking for " << std::fixed << std::setprecision(2) << elapsed_seconds.count() << " seconds \n";
      move_list.push_back(engine_move);
      player_turn = !player_turn;
    }

    std::cout << board;
    auto const color = board.get_active_color();
    if (board.is_in_checkmate(color))
    {
      game_in_progress = false;
      std::string winning_player = (color == Color::white ? std::string{"black"} : std::string{"white"});
      std::cout << winning_player << " is victorious!" << std::endl;

      std::cout << "Moves: ";
      for (auto const& move : move_list)
      {
        std::cout << move << " ";
      }
      std::cout << "\n";
    }
  }
}

void Game::computer_vs_computer()
{
  Meneldor_engine white_engine;
  white_engine.initialize();

  Meneldor_engine black_engine;
  black_engine.initialize();

  Board board;

  std::vector<std::string> move_list;
  bool white_to_move{true};
  bool game_in_progress{true};
  while (game_in_progress)
  {
    if (white_to_move)
    {
      std::cout << "White engine thinking\n";

      senjo::GoParams params;
      auto const start = std::chrono::system_clock::now();
      auto const engine_move = white_engine.go(params, nullptr);
      auto const end = std::chrono::system_clock::now();
      std::chrono::duration<double> const elapsed_seconds = end - start;

      [[maybe_unused]] auto result = board.try_move_uci(engine_move);
      MY_ASSERT(result.has_value(), "Engine cannot play illegal move");
      white_engine.makeMove(engine_move);
      black_engine.makeMove(engine_move);
      std::cout << "Engine played " << engine_move << " after thinking for " << std::fixed << std::setprecision(2) << elapsed_seconds.count() << " seconds \n";
      move_list.push_back(engine_move);
      white_to_move = !white_to_move;
    }
    else
    {
      std::cout << "Black engine thinking\n";

      senjo::GoParams params;
      auto const start = std::chrono::system_clock::now();
      auto const engine_move = black_engine.go(params, nullptr);
      auto const end = std::chrono::system_clock::now();
      std::chrono::duration<double> const elapsed_seconds = end - start;

      [[maybe_unused]] auto result = board.try_move_uci(engine_move);
      MY_ASSERT(result.has_value(), "Engine cannot play illegal move");
      white_engine.makeMove(engine_move);
      black_engine.makeMove(engine_move);
      std::cout << "Engine played " << engine_move << " after thinking for " << std::fixed << std::setprecision(2) << elapsed_seconds.count() << " seconds \n";
      move_list.push_back(engine_move);
      white_to_move = !white_to_move;
    }

    std::cout << "Board state after " << move_list.size() << " moves\n";
    std::cout << board;
    auto const color = board.get_active_color();
    if (board.is_in_checkmate(color))
    {
      game_in_progress = false;
      std::string winning_player = (color == Color::white ? std::string{"black"} : std::string{"white"});
      std::cout << winning_player << " is victorious!" << std::endl;

      std::cout << "Moves: ";
      for (auto const& move : move_list)
      {
        std::cout << move << " ";
      }
      std::cout << "\n";
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
