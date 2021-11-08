#!/usr/bin/env python3

import chess
import os
import sys


'''
Given a position
    get legal moves for that position
    for each move
        print(fen)
        return count of legal moves for that position

u64 Perft(int depth)
{
  MOVE move_list[256];
  int n_moves, i;
  u64 nodes = 0;

  if (depth == 0)
    return 1ULL;

  n_moves = GenerateLegalMoves(move_list);
  for (i = 0; i < n_moves; i++) {
    MakeMove(move_list[i]);
    nodes += Perft(depth - 1);
    UndoMove(move_list[i]);
  }
  return nodes;
}
'''


def perft_ish(board, depth):
    nodes = 0

    if depth == 0:
        return 1

    legal_moves = board.legal_moves
    for move in board.legal_moves:
        board.push(move)
        nodes += perft_ish(board, depth - 1)
        board.pop()


    print(f'{board.fen()}: {nodes} moves')

    return nodes


def main():

    fen = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
    board = chess.Board(fen)

    depth = 1
    if len(sys.argv) == 2:
        depth = int(sys.argv[1])

    result = perft_ish(board, depth)
    print(f'Perft({depth}) = {result}')


if __name__ == '__main__':
    main()
