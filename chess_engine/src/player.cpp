#include "player.h"
#include "board.h"
#include "coordinates.h"
#include "utils.h"

namespace
{
std::string move_to_string(Move m)
{
  std::stringstream ss;
  ss << m;
  return ss.str();
}

} // namespace

Player::Player(std::string name) : m_name(std::move(name))
{
}

std::string const& Player::get_name() const
{
  return m_name;
}

User_player::User_player(std::string name) : Player(std::move(name))
{
}

// Returns empty if the player resigns.
std::optional<std::string> User_player::get_next_move(std::istream& in, std::ostream& out)
{
  constexpr static bool uci_mode{false};

  std::string line = "";
  line.reserve(64);

  do
  {
    if (!in.good())
    {
      return {};
    }

    out << get_name() << ", please enter the beginning and ending squares of the move (ex: "
        << (uci_mode ? std::string{"e2e4"} : std::string{"Bxc7"}) << "): ";

    // Get move from the user and ensure that it is of the correct form
    getline(in, line);
    if (line == "quit")
    {
      // Empty if the player resigned
      return {};
    }

    if (auto move =
          uci_mode ? m_board.move_from_uci(line) : m_board.move_from_algebraic(line, m_board.get_active_color()))
    {
      return move_to_string(*move);
    }
  } while (true);
}

void User_player::notify(std::string const& move)
{
  m_board.try_move_uci(move);
}

bool User_player::set_position(std::string_view fen)
{
  if (auto board = Board::from_fen(fen))
  {
    m_board = *board;
    return true;
  }
  return false;
}

void User_player::reset()
{
  m_board = {};
}

Engine_player::Engine_player(std::string name) : Player(std::move(name))
{
  m_engine.initialize();
}

std::optional<std::string> Engine_player::get_next_move(std::istream& /* in */, std::ostream& out)
{
  out << get_name() << " thinking\n";
  senjo::GoParams params;
  auto const start = std::chrono::system_clock::now();
  auto const engine_move = m_engine.go(params, nullptr);
  auto const end = std::chrono::system_clock::now();
  std::chrono::duration<double> const elapsed_time = end - start;
  auto search_stats = m_engine.getSearchStats();

  out << "Engine played " << engine_move << " after thinking for " << std::fixed << std::setprecision(2)
      << format_with_commas(elapsed_time.count()) << " seconds and searching "
      << format_with_commas(search_stats.nodes) << " nodes ("
      << format_with_commas(search_stats.nodes / elapsed_time.count()) << " nodes/sec)\n";

  return engine_move;
}

void Engine_player::notify(std::string const& move)
{
  m_engine.makeMove(move);
}

bool Engine_player::set_position(std::string_view fen)
{
  return m_engine.setPosition(std::string{fen});
}

void Engine_player::reset()
{
  m_engine.resetEngineStats();
  m_engine.initialize();
}
