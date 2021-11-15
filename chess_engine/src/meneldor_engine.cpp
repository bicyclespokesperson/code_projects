#include "meneldor_engine.h"
#include "move_generator.h"

constexpr int c_default_depth{5};

int Meneldor_engine::evaluate(Board const& board, Color /* color */) const
{
  //color = Color::black;
  auto const multiplier = (board.get_active_color() == Color::white) ? 1 : -1;
  //auto const multiplier = 1;

  // These arrays can be iterated in parallel
  constexpr static std::array piece_values{1, 3, 3, 5, 9, 10};
  constexpr static std::array pieces{Piece::pawn, Piece::knight, Piece::bishop, Piece::rook, Piece::queen, Piece::king};
  static_assert(piece_values.size() == pieces.size());

  //TODO: Skipping these makes searching 6x faster. Optimize somehow, maybe
  //      by caching get_attacked_squares or something like that
  if (board.get_active_color() == Color::white)
  {
    if (board.is_in_checkmate(Color::white))
    {
      return multiplier * (std::numeric_limits<int>::min() + 1);
    }
    if (board.is_in_stalemate(Color::white))
    {
      return 0;
    }
  }

  if (board.get_active_color() == Color::black)
  {
    if (board.is_in_checkmate(Color::black))
    {
      return multiplier * (std::numeric_limits<int>::max() - 1);
    }
    if (board.is_in_stalemate(Color::black))
    {
      return 0;
    }
  }

  if (board.get_halfmove_clock() >= 50)
  {
    return 0;
  }

  //TODO: repetition, insufficient material

  std::array<int, pieces.size()> black_piece_counts{};
  std::transform(pieces.cbegin(), pieces.cend(), black_piece_counts.begin(),
                 [&](Piece piece)
                 {
                   return board.get_piece_set(Color::black, piece).occupancy();
                 });

  std::array<int, pieces.size()> white_piece_counts{};
  std::transform(pieces.cbegin(), pieces.cend(), white_piece_counts.begin(),
                 [&](Piece piece)
                 {
                   return board.get_piece_set(Color::white, piece).occupancy();
                 });

  auto const black_material =
    std::inner_product(black_piece_counts.cbegin(), black_piece_counts.cend(), piece_values.cbegin(), 0);
  auto const white_material =
    std::inner_product(white_piece_counts.cbegin(), white_piece_counts.cend(), piece_values.cbegin(), 0);

  //std::cout << "                             eval returning: " << multiplier * (white_material - black_material) << "\n";
  return multiplier * (white_material - black_material);
}

int Meneldor_engine::quiesce_(Board const& board, Color color, int /*alpha*/, int /*beta*/) const
{
  return evaluate(board, color);
}

int Meneldor_engine::quiesce_min_(Board const& board, Color color, int /* alpha */, int /* beta */) const
{
#if 0
  ++m_visited_nodes;
  //auto const indents = std::string(c_default_depth - depth_remaining + 1, ' ');

  auto const attacks = Move_generator::generate_legal_attack_moves(board);
  if (attacks.empty())
  {
    return evaluate(board, color);
  }
  for (auto move : attacks)
  {
    Board tmp_board{board};
    tmp_board.move_no_verify(move);
    int score = quiesce_max_(tmp_board, color, alpha, beta);
    //std::cout << indents << "Response (in min): " << move << " " << score << "\n";
    if (score <= alpha)
    {
      return alpha;
    }

    if (score < beta)
    {
      beta = score;
    }
  }
  return beta;
#else
  return evaluate(board, color);
#endif
}

int Meneldor_engine::quiesce_max_(Board const& board, Color color, int /* alpha */, int /* beta */) const
{
#if 0
  ++m_visited_nodes;

  //auto const indents = std::string(c_default_depth - depth_remaining + 1, ' ');

  auto const attacks = Move_generator::generate_legal_attack_moves(board);
  if (attacks.empty())
  {
    return evaluate(board, color);
  }
  for (auto move : attacks)
  {
    Board tmp_board{board};
    tmp_board.move_no_verify(move);
    int score = quiesce_min_(tmp_board, color, alpha, beta);
    //std::cout << indents << "Response (in max): " << move << " " << score << "\n";
    if (score >= beta)
    {
      return beta;
    }

    if (score > alpha)
    {
      alpha = score;
    }
  }
  return alpha;
#else
  return -evaluate(board, color);
#endif
}

int Meneldor_engine::alpha_beta_max_(Board const& board, Color color, int alpha, int beta, int depth_remaining)
{
  ++m_visited_nodes;
  auto const indents = std::string(c_default_depth - depth_remaining + 1, ' ');

  if (depth_remaining == 0)
  {
    return quiesce_min_(board, color, alpha, beta);
  }
  for (auto move : Move_generator::generate_legal_moves(board))
  {
    Board tmp_board{board};
    tmp_board.move_no_verify(move);
    int score = alpha_beta_min_(tmp_board, color, alpha, beta, depth_remaining - 1);
    //std::cout << indents << "response (in max): " << move << " " << score << "\n";
    if (score >= beta)
    {
      return beta;
    }

    if (score > alpha)
    {
      alpha = score;
    }
  }
  return alpha;
}

int Meneldor_engine::alpha_beta_min_(Board const& board, Color color, int alpha, int beta, int depth_remaining)
{
  ++m_visited_nodes;
  auto const indents = std::string(c_default_depth - depth_remaining + 1, ' ');

  if (depth_remaining == 0)
  {
    return -quiesce_max_(board, color, alpha, beta);
  }
  for (auto move : Move_generator::generate_legal_moves(board))
  {
    Board tmp_board{board};
    tmp_board.move_no_verify(move);
    int score = alpha_beta_max_(tmp_board, color, alpha, beta, depth_remaining - 1);
    //std::cout << indents << "Response (in min): " << move << " " << score << "\n";
    if (score <= alpha)
    {
      return alpha;
    }

    if (score < beta)
    {
      beta = score;
    }
  }
  return beta;
}

/*
 * On white's decision, it sets alpha
 *  alpha - white has an option to reach this
 * On black's decision, it sets beta
 *  beta - black has an option to reach this
 */

int Meneldor_engine::negamax_(Board& board, Color color, int alpha, int beta, int depth_remaining)
{
  ++m_visited_nodes;
  std::string indents;
  for (int i{0}; i < 1 + (c_default_depth - depth_remaining); ++i)
  {
    indents += " ";
  }

  if (depth_remaining == 0)
  {
    auto result = quiesce_(board, color, alpha, beta);

    return result;
  }

  auto moves = Move_generator::generate_legal_moves(board);
  m_orderer.sort_moves(moves, board);
  for (auto move : moves)
  {
    Board tmp_board{board};
    tmp_board.move_no_verify(move);
    int score = -negamax_(tmp_board, color, -beta, -alpha, depth_remaining - 1);
    if (score >= beta)
    {
      return beta; // Opposing player has a move that gives a worse position for us than our current best move
    }
    if (score > alpha)
    {
      alpha = score; // New best possible score
    }
  }

  return alpha;
}

Meneldor_engine::Meneldor_engine() = default;

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
    return true;
  }

  return false;
}

bool Meneldor_engine::makeMove(const std::string& move)
{
  if (m_board.try_move_uci(move))
  {
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

std::string Meneldor_engine::go(const senjo::GoParams& params, std::string* /* ponder = nullptr */)
{
  m_visited_nodes = 0;
  m_stop_requested.clear();
  m_is_searching.test_and_set();

  auto const color = m_board.get_active_color();
  auto legal_moves = Move_generator::generate_legal_moves(m_board);
  MY_ASSERT(!legal_moves.empty(), "Already in checkmate or stalemate");
  m_orderer.sort_moves(legal_moves, m_board);

  const int depth = (params.depth > 0) ? params.depth : c_default_depth;

  //std::cout << "Eval of current position (for " << color << "): " << std::to_string(evaluate(m_board, color)) << "\n";
  std::pair<Move, int> best{legal_moves.front(), std::numeric_limits<int>::min()};
  for (auto move : legal_moves)
  {
    auto tmp_board = m_board;
    tmp_board.move_no_verify(move);

    //auto const score = alpha_beta_max_(tmp_board, color, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), depth);

    // Stay one away from the limits so negating the values doesn't cause problems
    auto positive_inf = std::numeric_limits<int>::max() - 1;
    auto negative_inf = std::numeric_limits<int>::min() + 1;
    auto const score = -negamax_(tmp_board, color, -positive_inf, -negative_inf, depth);

    //auto const score = alpha_beta_min_(tmp_board, color, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), depth);
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

  std::stringstream ss;
  ss << best.first;
  m_is_searching.clear();
  return ss.str();
}

senjo::SearchStats Meneldor_engine::getSearchStats() const
{
  senjo::SearchStats result;

  result.depth = c_default_depth;
  result.seldepth = c_default_depth;
  result.nodes = m_visited_nodes;
  result.qnodes = 0; // TODO: Update once engine supports quiescence searching
  result.msecs = 0; // TODO: Update once engine supports time

  return result;
}

uint32_t Meneldor_engine::previous_move_nodes_searched() const
{
  return m_visited_nodes;
}

void Meneldor_engine::resetEngineStats()
{
  // Called when "test" command is received
}

void Meneldor_engine::showEngineStats() const
{
  // Called when "test" command is received
}
