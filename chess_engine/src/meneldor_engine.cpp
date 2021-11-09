#include "meneldor_engine.h"
#include "move_generator.h"

constexpr int c_default_depth{1};

int Meneldor_engine::evaluate(Board const& board, Color color) const
{
  //color = Color::black;

  // These arrays can be iterated in parallel
  constexpr static std::array piece_values{1, 3, 3, 5, 9, 10};
  constexpr static std::array pieces{Piece::pawn, Piece::knight, Piece::bishop, Piece::rook, Piece::queen, Piece::king};
  static_assert(piece_values.size() == pieces.size());

  if (board.is_in_checkmate(opposite_color(color)))
  {
    return std::numeric_limits<int>::max();
  }

  std::array<int, pieces.size()> opposing_piece_counts{};
  std::transform(pieces.cbegin(), pieces.cend(), opposing_piece_counts.begin(),
                 [&](Piece piece)
                 {
                   return board.get_piece_set(opposite_color(color), piece).occupancy();
                 });

  std::array<int, pieces.size()> piece_counts{};
  std::transform(pieces.cbegin(), pieces.cend(), piece_counts.begin(),
                 [&](Piece piece)
                 {
                   return board.get_piece_set(color, piece).occupancy();
                 });

  auto const opposing_material =
    std::inner_product(opposing_piece_counts.cbegin(), opposing_piece_counts.cend(), piece_values.cbegin(), 0);
  auto const material = std::inner_product(piece_counts.cbegin(), piece_counts.cend(), piece_values.cbegin(), 0);

  return (material - opposing_material);
}

int Meneldor_engine::quiesce_(Board const& board, Color color) const
{
  // TODO: Actually implement quiescence search
  return evaluate(board, color);
}

// Find my best move
int Meneldor_engine::alpha_beta_max_(Board const& board, Color color, int alpha, int beta, int depth_remaining ) const {
  std::string indents;
  for (int i{0}; i < 1 + (c_default_depth - depth_remaining); ++i)
  {
    indents += " ";
  }

   if ( depth_remaining == 0 )
   {
     return quiesce_(board, color);
   }

   for (auto move : Move_generator::generate_legal_moves(board))
   {
      Board tmp_board{board};
      tmp_board.move_no_verify(move);
      auto score = alpha_beta_max_(tmp_board, opposite_color(color), beta, alpha, depth_remaining - 1 );
      std::cout << indents << " Response " << move << ": " << score << "\n";

      if( score >= beta )
      {
         return beta;
      }
      if( score > alpha )
      {
         alpha = score;
      }
   }
   return alpha;
}

// Find their best move
int Meneldor_engine::alpha_beta_min_(Board const& board, Color color, int alpha, int beta, int depth_remaining ) const {
   if ( depth_remaining == 0 ) 
   {
     return -quiesce_(board, color);
   }

   for (auto move : Move_generator::generate_legal_moves(board))
   {
      Board tmp_board{board};
      tmp_board.move_no_verify(move);
      auto score = alpha_beta_min_(tmp_board, color, alpha, beta, depth_remaining - 1 );
      if( score <= alpha )
      {
         return alpha;
      }
      if( score < beta )
      {
         beta = score;
      }
   }
   return beta;
}

// alpha - best result for active player
// beta - best result for opposing player
int Meneldor_engine::alpha_beta_(Board& board, Color color, int alpha, int beta, int depth_remaining)
{
  std::string indents;
  for (int i{0}; i < 1 + (c_default_depth - depth_remaining); ++i)
  {
    indents += " ";
  }

  if (depth_remaining == 0)
  {
    auto result = quiesce_(board, color);
    std::cout << indents << std::to_string(result) << "\n";
     
    return result;
  }

  for (auto move : Move_generator::generate_legal_moves(board))
  {
    std::cout << indents << " Response " << move << " ";
    Board tmp_board{board};
    tmp_board.move_no_verify(move);
    int score = alpha_beta_(tmp_board, opposite_color(color), beta, alpha, depth_remaining - 1);
    std::cout << std::to_string(score) << "\n";
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
  MY_ASSERT(false, "Not implemented");
  return {};
}

bool Meneldor_engine::setEngineOption(const std::string& /* optionName */, const std::string& /* optionValue */)
{
  MY_ASSERT(false, "Not implemented");
  return true;
}

void Meneldor_engine::initialize()
{
  m_board = Board{};
}

bool Meneldor_engine::isInitialized() const
{
  //MY_ASSERT(false, "Not implemented");
  return true;
}

bool Meneldor_engine::setPosition(const std::string& fen, std::string* /* remain  = nullptr */)
{
  std::cout << "Setting position to " << fen << "\n";
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

    auto const score = alpha_beta_max_(tmp_board, color, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), depth);
    //auto const score = alpha_beta_(tmp_board, color, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), depth);
    std::cout << "Evaluating move: " << move << ", score: " << std::to_string(score) << "\n";

    if (score > best.second)
    {
      best = {move, score};
      std::cout << "Updating best move to: " << move;
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
