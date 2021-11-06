/*
 * File:   chess.cpp
 * Author: 1697518
 *
 * This is the main method for the chess game.
 * Created on March 24, 2013, 5:02 PM
 */

#include "chess.h"
#include "bitboard.h"
#include "board.h"
#include "game.h"
#include "meneldor_engine.h"
#include "my_assert.h"
#include "player.h"
#include "move_generator.h"
#include <sstream>

void print_bitboard_with_squares(std::vector<std::string> const& squares)
{
  Bitboard bb;
  for (auto const& str : squares)
  {
    if (auto sq = Coordinates::from_str(str))
    {
      bb.set_square(*sq);
    }
    else
    {
      std::cout << "Invalid square: " << str << "\n";
    }
  }
  std::cout << bb << "\nhex: " << bb.hex_str() << "\n";
}

uint64_t Perft(int depth, Board& board)
{
  std::vector<Bitboard> en_passant_squares;
  en_passant_squares.reserve(256);

  std::vector<Board::Castling_rights> castling_rights;
  castling_rights.reserve(256);

  uint64_t nodes{0};

  if (depth == 0) 
  {
    return uint64_t{1};
  }

  auto color = board.get_active_color();
  auto moves = Move_generator::generate_pseudo_legal_moves(board, color);
  for (auto m : moves)
  {

    en_passant_squares.push_back(board.get_en_passant_square());
    castling_rights.push_back(board.get_castling_rights());

    auto captured_piece = board.try_move(m);
    if (!captured_piece.has_value())
    {
      std::cout << board << "\n" << board.to_fen() << "\n";
    }
    MY_ASSERT(captured_piece.has_value(), "Invalid move");
    //auto captured_piece = board.move_no_verify(m);
    if (!board.is_in_check(color))
      nodes += Perft(depth - 1, board);

    board.undo_move(m, *captured_piece, en_passant_squares.back(), castling_rights.back());
    en_passant_squares.pop_back();
    castling_rights.pop_back();
  }
  return nodes;
}

/**
 * Play the chess game.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
#if 0
  Game game;
  game.run();
#endif

  int depth{2};
  if (argc == 2)
  {
    depth = std::stoi(argv[1]);
  }

  // Position 2 as described here. Perft(1) should be 48, currently returning 46
  // https://www.chessprogramming.org/Perft_Results#Position_2
  std::string fen_position_2{"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"};

  std::string fen_2_problem_board{"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/R3K2R b KQkq - 0 1"};

  std::string fen_position_3{"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"};

  std::string fen_position_5{"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"};

  Board board = *Board::from_fen(fen_position_2);
  //Board board;

  

  int result = Perft(depth, board);
  std::cout << "Perft(" << std::to_string(depth) << ") = " << std::to_string(result) << "\n";

  return 0;
}
