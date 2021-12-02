#ifndef UCI_ENGINE_PLAYER_H
#define UCI_ENGINE_PLAYER_H

#include "player.h"

class Uci_engine_player : public Player
{
public:
  Uci_engine_player(std::string name, std::string engine_path);

  ~Uci_engine_player() override;

  std::optional<std::string> get_next_move(std::istream& in, std::ostream& out) override;

  void notify(std::string const& move) override;

  bool set_position(std::string_view fen) override;

  void reset() override;

private:
  bool send_message_(std::string_view msg);
  std::string receive_message_();
  void terminate_engine_process_();
  bool init_engine_();

  std::string m_engine_path{};
  std::string m_starting_position_fen{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};

  std::array<int, 2> m_to_child{};
  std::array<int, 2> m_from_child{};
  pid_t m_child_pid{};
};

#endif // UCI_ENGINE_PLAYER_H
