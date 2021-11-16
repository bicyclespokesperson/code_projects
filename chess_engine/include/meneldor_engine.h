#ifndef MENELDOR_ENGINE
#define MENELDOR_ENGINE

#include "bitboard.h"
#include "board.h"
#include "chess_types.h"
#include "coordinates.h"
#include "move_orderer.h"
#include "senjo/ChessEngine.h"
#include "transposition_table.h"

class Meneldor_engine : public senjo::ChessEngine
{
public:
  Meneldor_engine();

  std::string getEngineName() const override;

  std::string getEngineVersion() const override;

  std::string getAuthorName() const override;

  std::string getEmailAddress() const override;

  std::string getCountryName() const override;

  std::list<senjo::EngineOption> getOptions() const override;

  bool setEngineOption(const std::string& optionName, const std::string& optionValue) override;

  void initialize() override;

  bool isInitialized() const override;

  bool setPosition(const std::string& fen, std::string* remain = nullptr) override;

  bool makeMove(const std::string& move) override;

  std::string getFEN() const override;

  void printBoard() const override;

  bool whiteToMove() const override;

  void clearSearchData() override;

  void ponderHit() override;

  bool isRegistered() const override;

  void registerLater() override;

  bool doRegistration(const std::string& name, const std::string& code) override;

  bool isCopyProtected() const override;

  bool copyIsOK() override;

  void setDebug(const bool flag) override;

  bool isDebugOn() const override;

  bool isSearching() override;

  void stopSearching() override;

  bool stopRequested() const override;

  void waitForSearchFinish() override;

  uint64_t perft(const int depth) override;

  std::string go(const senjo::GoParams& params, std::string* ponder = nullptr) override;

  senjo::SearchStats getSearchStats() const override;

  void resetEngineStats() override;

  void showEngineStats() const override;

  int evaluate(Board const& board) const;

  uint32_t previous_move_nodes_searched() const;

private:
  int negamax_(Board& board, int alpha, int beta, int depth_remaining);

  int quiesce_(Board const& board, int alpha, int beta) const;

  bool m_is_debug{true};
  std::atomic_flag m_stop_requested{false};
  std::atomic_flag m_is_searching{false};
  Board m_board;
  Move_orderer m_orderer{};
  constexpr static int c_default_depth{5}; //TODO: This should be a property on the engine
  int m_depth_for_current_search{c_default_depth};

  constexpr static size_t c_transposition_table_size_bytes{1024UL * 1024UL * 1024UL}; // TODO: Greatly increase
  Transposition_table m_transpositions{c_transposition_table_size_bytes};

  mutable uint32_t m_visited_nodes{0};
};

#endif // MENELDOR_ENGINE
