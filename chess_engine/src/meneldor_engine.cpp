#include "meneldor_engine.h"
#include "move_generator.h"
#include "utils.h"

namespace
{
constexpr int positive_inf = std::numeric_limits<int>::max();
constexpr int negative_inf = -positive_inf;
static_assert(positive_inf == -negative_inf, "Values should be inverses of each other");

int tt_hits{0};
int tt_misses{0};
int tt_sufficient_depth{0};

} // namespace

// Returns a number that is positive if the side to move is winning, and negative if losing
int Meneldor_engine::evaluate(Board const& board) const
{
  // These arrays can be iterated in parallel
  constexpr static std::array piece_values{100, 300, 300, 500, 900};
  constexpr static std::array pieces{Piece::pawn, Piece::knight, Piece::bishop, Piece::rook, Piece::queen};
  static_assert(piece_values.size() == pieces.size());

  auto const state = board.calc_game_state();
  if (state == Game_state::white_victory || state == Game_state::black_victory)
  {
    return negative_inf +
           m_depth_for_current_search; // Add depth so the search function can return a slightly higher value if it finds an earlier mate
  }
  if (state == Game_state::draw)
  {
    return c_contempt_score;
  }

  if (board.get_halfmove_clock() >= 100)
  {
    return c_contempt_score;
  }

  auto const color = board.get_active_color();
  auto const enemy_color = opposite_color(board.get_active_color());
  int result{0};
  for (size_t i{0}; i < pieces.size(); ++i)
  {
    result += (board.get_piece_set(color, pieces[i]).occupancy() - board.get_piece_set(enemy_color, pieces[i]).occupancy()) * piece_values[i];
  }

  // Positions that can attack more squares are better
  result += Move_generator::get_all_attacked_squares(board, board.get_active_color()).occupancy();

  return result;
}

int Meneldor_engine::quiesce_(Board const& board, int alpha, int beta) const
{
  ++m_visited_quiesence_nodes;
  auto score = evaluate(board);
  if (score >= beta)
  {
    return beta;
  }
  alpha = std::max(alpha, score);

  auto moves = Move_generator::generate_legal_attack_moves(board);
  m_orderer.sort_moves(moves, board);

  Board tmp_board{board};
  for (auto const move : moves)
  {
    tmp_board = board;
    tmp_board.move_no_verify(move);
    auto const score = -quiesce_(tmp_board, -beta, -alpha);

    if (score >= beta)
    {
      return beta;
    }
    alpha = std::max(score, alpha);
  }

  return alpha;
}

int Meneldor_engine::negamax_(Board& board, int alpha, int beta, int depth_remaining)
{
  ++m_visited_nodes;

  if (depth_remaining == 0)
  {
    auto result = quiesce_(board, alpha, beta);
    return result;
  }

  if (std::find(m_previous_positions.cbegin(), m_previous_positions.cend(), board.get_hash_key()) !=
      m_previous_positions.cend())
  {
    return c_contempt_score; // Draw by repetition
  }

  if (auto entry = m_transpositions.get(board.get_hash_key()))
  {
    ++tt_hits;
    if (entry->depth >= depth_remaining)
    {
      ++tt_sufficient_depth;
      switch (entry->type)
      {
        case Transposition_table::Eval_type::alpha:
          // Eval_type::alpha implies we didn't find a move from this position that was as good as a move that
          // we could have made earlier to lead to a different position. That means the position has an evaluation
          // that is at most "entry.evaluation"
          beta = std::min(beta, entry->evaluation);
          break;
        case Transposition_table::Eval_type::beta:
          // Eval_type::beta implies that we stopped evaluating last time because we didn't think the opposing player
          // would allow this position to be reached. That means the position has an evaluation of at least "entry.evaluation",
          // but there may be an even better move that was skipped
          alpha = std::max(alpha, entry->evaluation);
          break;
        case Transposition_table::Eval_type::exact:
          return entry->evaluation;
          break;
      }
    }
  }
  else
  {
    ++tt_misses;
  }

  auto moves = Move_generator::generate_legal_moves(board);
  if (moves.empty())
  {
    if (board.is_in_check(board.get_active_color()))
    {
      return negative_inf + (m_depth_for_current_search - depth_remaining);
    }
    return 0;
  }
  m_orderer.sort_moves(moves, board);

  // If we don't find a move here that's better than alpha, just save alpha as the upper bound for this position
  auto eval_type = Transposition_table::Eval_type::alpha;
  Move best{moves.front()};
  Board tmp_board{board};
  for (auto const move : moves)
  {
    tmp_board = board;
    tmp_board.move_no_verify(move);
    auto const score = -negamax_(tmp_board, -beta, -alpha, depth_remaining - 1);

    if (score >= beta)
    {
      // Stop evaluating here since the opposing player won't let us get even this position on their previous move
      // Our evaluation here is a lower bound
      auto eval_type = Transposition_table::Eval_type::beta;
      m_transpositions.insert(board.get_hash_key(), {board.get_hash_key(), depth_remaining, score, move, eval_type});

      return beta;
    }

    if (score > alpha)
    {
      alpha = score;
      best = move;

      // If this is never hit, we know that the best alpha can be is the alpha that was passed into the function
      eval_type = Transposition_table::Eval_type::exact;
    }
  }

  m_transpositions.insert(board.get_hash_key(), {board.get_hash_key(), depth_remaining, alpha, best, eval_type});
  return alpha;
}

Meneldor_engine::Meneldor_engine()
{
  m_previous_positions.reserve(256);
}

std::string Meneldor_engine::getEngineName() const
{
  return "Meneldor";
}

std::string Meneldor_engine::getEngineVersion() const
{
  return "0.1";
}

std::string Meneldor_engine::getAuthorName() const
{
  return "Jeremy Sigrist";
}

std::string Meneldor_engine::getEmailAddress() const
{
  return "";
}

std::string Meneldor_engine::getCountryName() const
{
  return "USA";
}

std::list<senjo::EngineOption> Meneldor_engine::getOptions() const
{
  // This engine does not currently advertise any options / settings
  return {};
}

bool Meneldor_engine::setEngineOption(const std::string& /* optionName */, const std::string& /* optionValue */)
{
  return false;
}

void Meneldor_engine::initialize()
{
  m_board = {};
}

bool Meneldor_engine::isInitialized() const
{
  return true;
}

bool Meneldor_engine::setPosition(const std::string& fen, std::string* /* remain  = nullptr */)
{
  //TODO: Use Output() to report errors in fen string
  if (auto board = Board::from_fen(fen))
  {
    m_board = *board;
    m_previous_positions.clear();
    return true;
  }

  return false;
}

bool Meneldor_engine::makeMove(const std::string& move)
{
  if (m_board.try_move_uci(move))
  {
    if (m_board.get_halfmove_clock() == 0)
    {
      // If the halfmove clock was zero we just had a capture or pawn move, both
      // of which make repeating positions impossible. That means we can't repeat
      // any of the positions in the list, so clear it so we don't have to compare
      // against them going forward
      m_previous_positions.clear();
    }

    m_previous_positions.push_back(m_board.get_hash_key());
    return true;
  }

  return false;
}

std::string Meneldor_engine::getFEN() const
{
  return m_board.to_fen();
}

void Meneldor_engine::printBoard() const
{
  std::cout << m_board;
}

bool Meneldor_engine::whiteToMove() const
{
  return m_board.get_active_color() == Color::white;
}

void Meneldor_engine::clearSearchData()
{
  // No data persists between searches currently
}

void Meneldor_engine::ponderHit()
{
  // Notifies the engine that the ponder move was played
}

bool Meneldor_engine::isRegistered() const
{
  // This engine does not need to be registered to function
  return true;
}

void Meneldor_engine::registerLater()
{
  // This engine does not need to be registered to function
}

bool Meneldor_engine::doRegistration(const std::string& /* name */, const std::string& /* code */)
{
  // This engine does not need to be registered to function
  return true;
}

bool Meneldor_engine::isCopyProtected() const
{
  return false;
}

bool Meneldor_engine::copyIsOK()
{
  return true;
}

void Meneldor_engine::setDebug(const bool flag)
{
  m_is_debug = flag;
}

bool Meneldor_engine::isDebugOn() const
{
  return m_is_debug;
}

bool Meneldor_engine::isSearching()
{
  return m_is_searching.test();
}

void Meneldor_engine::stopSearching()
{
  m_stop_requested.test_and_set();
}

bool Meneldor_engine::stopRequested() const
{
  return m_stop_requested.test();
}

void Meneldor_engine::waitForSearchFinish()
{
  constexpr bool old_value{true};
  m_is_searching.wait(old_value);
}

uint64_t Meneldor_engine::perft(const int depth)
{
  m_stop_requested.clear();
  return Move_generator::perft(depth, m_board, m_stop_requested);
}

Move Meneldor_engine::search(int depth)
{
  auto legal_moves = Move_generator::generate_legal_moves(m_board);
  MY_ASSERT(!legal_moves.empty(), "Already in checkmate or stalemate");
  m_orderer.sort_moves(legal_moves, m_board);

  std::pair<Move, int> best{legal_moves.front(), negative_inf};
  for (auto move : legal_moves)
  {
    auto tmp_board = m_board;
    tmp_board.move_no_verify(move);

    auto const score = -negamax_(tmp_board, negative_inf, positive_inf, depth);

    if (m_is_debug)
    {
      std::cout << "Evaluating move: " << move << ", score: " << std::to_string(score) << "\n";
    }

    if (score > best.second)
    {
      best = {move, score};
    }

    if (m_stop_requested.test())
    {
      break;
    }
  }

  return best.first;
}

std::string Meneldor_engine::go(const senjo::GoParams& params, std::string* /* ponder = nullptr */)
{
  auto const start_time = std::chrono::system_clock::now();

  m_visited_nodes = 0;
  m_visited_quiesence_nodes = 0;
  m_stop_requested.clear();
  m_is_searching.test_and_set();

  if (m_is_debug)
  {
    auto const color = m_board.get_active_color();
    std::cout << "Eval of current position (for " << color << "): " << std::to_string(evaluate(m_board)) << "\n";
  }

  auto time_per_move = std::chrono::duration<double>{1.5};

  int max_depth = (params.depth > 0) ? params.depth : c_default_depth;
  Move best_move;
  for (int depth{2}; depth <= max_depth; ++depth)
  {
    m_depth_for_current_search = depth;
    best_move = search(m_depth_for_current_search);

    auto const current_time = std::chrono::system_clock::now();
    std::chrono::duration<double> const elapsed_time = current_time - start_time;
    if (elapsed_time > time_per_move)
    {
      if (m_is_debug)
      {
        std::cout << "Searched to depth: " << depth << "\n";
      }
      break;
    }
  }

  auto const end_time = std::chrono::system_clock::now();
  std::chrono::duration<double> const elapsed_seconds = end_time - start_time;
  std::stringstream ss;
  ss << best_move;

  if (m_is_debug)
  {
    std::cout << "Search depth: " << m_depth_for_current_search << "\n";
    std::cout << "TT occupancy: " << m_transpositions.count() << "\n";
    std::cout << "TT percent full: " << (static_cast<float>(m_transpositions.count()) / m_transpositions.get_capacity())
              << "\n";

    std::cout << "TT Hits: " << tt_hits << ", total: " << (tt_hits + tt_misses)
              << ", sufficient_depth: " << tt_sufficient_depth
              << ", hit%: " << (static_cast<float>(tt_hits) / (tt_hits + tt_misses)) << "\n";

    auto search_stats = getSearchStats();
    std::cout << "For position: " << getFEN() << "\n  Engine found " << ss.str() << " after thinking for " << std::fixed
        << std::setprecision(2) << format_with_commas(elapsed_seconds.count()) << " seconds and searching "
        << format_with_commas(search_stats.nodes) << " nodes ("
        << format_with_commas(search_stats.nodes / elapsed_seconds.count()) << " nodes/sec)\n"
        << "  QNodes searched: " << format_with_commas(search_stats.qnodes) << "\n";

    tt_hits = 0;
    tt_misses = 0;
    tt_sufficient_depth = 0;
  }

  m_is_searching.clear();
  return ss.str();
}

senjo::SearchStats Meneldor_engine::getSearchStats() const
{
  senjo::SearchStats result;

  result.depth = m_depth_for_current_search;
  result.seldepth = m_depth_for_current_search;
  result.nodes = m_visited_nodes;
  result.qnodes = m_visited_quiesence_nodes;
  result.msecs = 0; // TODO: Update once engine supports time

  return result;
}

void Meneldor_engine::resetEngineStats()
{
  // Called when "test" command is received
}

void Meneldor_engine::showEngineStats() const
{
  // Called when "test" command is received
}

// Should be called after go() but before makeMove()
bool Meneldor_engine::try_print_principle_variation(std::string move_str) const
{
  Board tmp_board{m_board};
  auto next_move = tmp_board.move_from_uci(std::move(move_str));

  std::cout << "Next move: " << *next_move << "\n";
  auto depth = m_depth_for_current_search;
  while (depth > 1)
  {
    if (!tmp_board.try_move(*next_move))
    {
      std::cout << "Could not find move in transposition table\n";
      return false;
    }

    auto entry = m_transpositions.get(tmp_board.get_hash_key());
    if (!entry)
    {
      std::cout << "Could not find next move in transposition table\n";
      return false;
    }
    depth = entry->depth;
    next_move = entry->best_move;
    std::cout << "Next move: " << *next_move;

    switch (entry->type)
    {
      case Transposition_table::Eval_type::alpha:
        // Eval_type::alpha implies we didn't find a move from this position that was as good as a move that
        // we could have made earlier to lead to a different position. That means the position has an evaluation
        // that is at most "entry.evaluation"
        std::cout << " (upper)";
        break;
      case Transposition_table::Eval_type::beta:
        // Eval_type::beta implies that we stopped evaluating last time because we didn't think the opposing player
        // would allow this position to be reached. That means the position has an evaluation of at least "entry.evaluation",
        // but there may be an even better move that was skipped
        std::cout << " (lower)";
        break;
      case Transposition_table::Eval_type::exact:
        std::cout << " (exact)";
        break;
    }
    std::cout << "\n";
  }

  return true;
}
