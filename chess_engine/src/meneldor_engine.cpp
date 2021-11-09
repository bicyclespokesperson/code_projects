#include "meneldor_engine.h"
#include "move_generator.h"

Meneldor_engine::Meneldor_engine()
{
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

std::string Meneldor_engine::go(const senjo::GoParams& /* params */, std::string* /* ponder = nullptr */)
{
  m_stop_requested.clear();
  MY_ASSERT(false, "Not implemented");
  return 0;
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
