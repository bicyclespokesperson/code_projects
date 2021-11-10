#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"
#include "chess_types.h"
#include "meneldor_engine.h"

/**
 * A player class represents one of the players in a chess game. It
 * can keep track of its pieces and make a move.
 */
class Player
{
public:
  virtual ~Player() = default;

  std::string const& get_name() const;

  // Called when a move is made successfully, either by this player or the opponent
  virtual void notify(std::string const& move) = 0;

  // Return empty if the player resigns
  virtual std::optional<std::string> get_next_move(std::istream& in, std::ostream& out) = 0;

  // Called at the start of a new game
  virtual void reset() = 0;

protected:
  Player(std::string name);

private:
  std::string m_name;
};

class Engine_player : public Player
{
public:
  Engine_player(std::string name);

  ~Engine_player() override = default;

  std::optional<std::string> get_next_move(std::istream& in, std::ostream& out) override;

  void notify(std::string const& move) override;

  void reset() override;

private:
  Meneldor_engine m_engine{};
};

class User_player : public Player
{
public:
  User_player(std::string name);

  ~User_player() override = default;

  std::optional<std::string> get_next_move(std::istream& in, std::ostream& out) override;

  void notify(std::string const& move) override;

  void reset() override;

private:
  Board m_board{};
};

#endif
