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


def perft(board, depth, print_moves=False):
    nodes = 0

    if depth == 0:
        return 1

    legal_moves = board.legal_moves
    for move in board.legal_moves:
        board.push(move)
        nodes += perft(board, depth - 1, print_moves)

        if print_moves:
            print(f'Move {move}')
        board.pop()


    print(f'{board.fen()}: {nodes} moves')

    return nodes


def main():

    fen_position_1 = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
    fen_position_2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"
    fen_position_3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"
    fen_position_4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"
    pos_4_sub_1 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1R1K b kq - 1 1"
    fen_position_5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"
    fen_position_6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "

    board = chess.Board(pos_4_sub_1)

    depth = 1
    if len(sys.argv) == 2:
        depth = int(sys.argv[1])

    print_moves = True
    result = perft(board, depth, print_moves)
    print(f'Perft({depth}) = {result}')


if __name__ == '__main__':
    main()
