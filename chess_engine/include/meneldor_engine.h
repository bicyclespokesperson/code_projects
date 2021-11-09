#ifndef MENELDOR_ENGINE
#define MENELDOR_ENGINE

#include "bitboard.h"
#include "board.h"
#include "chess_types.h"
#include "coordinates.h"
#include "senjo/ChessEngine.h"

class Meneldor_engine : public senjo::ChessEngine
{
public:
  Meneldor_engine();

  std::string getEngineName() const override = 0;

  std::string getEngineVersion() const override = 0;

  std::string getAuthorName() const override = 0;

  std::string getEmailAddress() const override;

  std::string getCountryName() const override;

  std::list<senjo::EngineOption> getOptions() const override = 0;

  bool setEngineOption(const std::string& optionName,
                               const std::string& optionValue) override = 0;

  void initialize() override = 0;

   bool isInitialized() const override = 0;

   bool setPosition(const std::string& fen,
                           std::string* remain = nullptr) override = 0;

   bool makeMove(const std::string& move) override = 0;

   std::string getFEN() const  override= 0;

   void printBoard() const override = 0;

   bool whiteToMove() const override = 0;

   void clearSearchData() override = 0;

   void ponderHit() override = 0;

   bool isRegistered() const override;

   void registerLater() override;

   bool doRegistration(const std::string& name,
                              const std::string& code) override;
   bool isCopyProtected() const override;

   bool copyIsOK() override;

   void setDebug(const bool flag) override = 0;

   bool isDebugOn() const override = 0;

   bool isSearching() override = 0;

   void stopSearching() override = 0;

   bool stopRequested() const override = 0;

   void waitForSearchFinish() override = 0;

   uint64_t perft(const int depth) override = 0;

   std::string go(const senjo::GoParams& params,
                         std::string* ponder = nullptr) override = 0;

   senjo::SearchStats getSearchStats() const override = 0;

   void resetEngineStats() override ;

   void showEngineStats() const override;

private:
   bool m_is_debug{false};
   std::atomic_flag m_stop_requested{false};
   Board m_board;
};

#endif // MENELDOR_ENGINE
