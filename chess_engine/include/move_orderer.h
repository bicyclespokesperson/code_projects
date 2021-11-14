#ifndef MOVE_ORDERER_H
#define MOVE_ORDERER_H

#include "chess_types.h"
#include "move.h"

class Board;

class Move_orderer
{
public:
  Move_orderer() = default;
  ~Move_orderer() = default;

  void sort_moves(std::span<Move> moves, Board const& board) const;

private:
};

#endif // MOVE_ORDERER_H
