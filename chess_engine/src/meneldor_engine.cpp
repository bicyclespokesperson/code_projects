#include "meneldor_engine.h"
#include "move_generator.h"

constexpr int c_default_depth{4};

int Meneldor_engine::evaluate(Board const& board, Color /* color */) const
{
  //color = Color::black;
  auto const multiplier = (board.get_active_color() == Color::white) ? 1 : -1;

  // These arrays can be iterated in parallel
  constexpr static std::array piece_values{1, 3, 3, 5, 9, 10};
  constexpr static std::array pieces{Piece::pawn, Piece::knight, Piece::bishop, Piece::rook, Piece::queen, Piece::king};
  static_assert(piece_values.size() == pieces.size());

  //TODO: Optimize these to make fewer total calls to Move_generator
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

  //TODO: repitition, insufficient material

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

  auto const black_material = std::inner_product(black_piece_counts.cbegin(), black_piece_counts.cend(), piece_values.cbegin(), 0);
  auto const white_material = std::inner_product(white_piece_counts.cbegin(), white_piece_counts.cend(), piece_values.cbegin(), 0);

  return multiplier * (white_material - black_material);
}

int Meneldor_engine::quiesce_(Board const& board, Color color, int /* alpha */, int /* beta */) const
{
  int eval = evaluate(board, color);
#if 0
  if (eval >= beta)
  {
    return beta;
  }
  if (alpha > eval)
  {
    alpha = eval;
  }

  // TODO: Actually implement quiescence search
  return alpha;
#endif
  return eval;
}

/*
 * On white's decision, it sets alpha
 *  alpha - white has an option to reach this
 * On black's decision, it sets beta
 *  beta - black has an option to reach this
 */

int Meneldor_engine::negamax_(Board& board, Color color, int alpha, int beta, int depth_remaining)
{
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

  for (auto move : Move_generator::generate_legal_moves(board))
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
  //MY_ASSERT(false, "Not implemented");
  return {};
}

bool Meneldor_engine::setEngineOption(const std::string& /* optionName */, const std::string& /* optionValue */)
{
  //MY_ASSERT(false, "Not implemented");
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
  MY_ASSERT(false, "Not implemented");
}

void Meneldor_engine::ponderHit()
{
  MY_ASSERT(false, "Not implemented");
}

bool Meneldor_engine::isRegistered() const
{
  MY_ASSERT(false, "Not implemented");
  return false;
}

void Meneldor_engine::registerLater()
{
  MY_ASSERT(false, "Not implemented");
}

bool Meneldor_engine::doRegistration(const std::string& /* name */, const std::string& /* code */)
{
  MY_ASSERT(false, "Not implemented");
  return false;
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
  MY_ASSERT(false, "Not implemented");
  return false;
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
  MY_ASSERT(false, "Not implemented");
}

uint64_t Meneldor_engine::perft(const int depth)
{
  m_stop_requested.clear();
  return Move_generator::perft(depth, m_board, m_stop_requested);
}

std::string Meneldor_engine::go(const senjo::GoParams& params, std::string* /* ponder = nullptr */)
{
  m_stop_requested.clear();

  auto const color = m_board.get_active_color();
  auto const legal_moves = Move_generator::generate_legal_moves(m_board);
  MY_ASSERT(!legal_moves.empty(), "Already in checkmate or stalemate");

  const int depth = (params.depth > 0) ? params.depth : c_default_depth;

  std::cout << "Eval of current position (for " << color << "): " << std::to_string(evaluate(m_board, color)) << "\n";
  std::pair<Move, int> best{legal_moves.front(), std::numeric_limits<int>::min()};
  for (auto move : legal_moves)
  {
    auto tmp_board = m_board;
    tmp_board.move_no_verify(move);

    //auto const score = alpha_beta_max_(tmp_board, color, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), depth);

    auto positive_inf = std::numeric_limits<int>::max() - 1;
    auto negative_inf = std::numeric_limits<int>::min() + 1;

    auto const score = -negamax_(tmp_board, color, -positive_inf, -negative_inf, depth);
    //std::cout << "Evaluating move: " << move << ", score: " << std::to_string(score) << "\n";

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
  return ss.str();
}

senjo::SearchStats Meneldor_engine::getSearchStats() const
{
  MY_ASSERT(false, "Not implemented");
  return {};
}

void Meneldor_engine::resetEngineStats()
{
  // Called when "test" command is received
}

void Meneldor_engine::showEngineStats() const
{
  // Called when "test" command is received
}
