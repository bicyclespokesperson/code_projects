#include "game.h"
#include "bishop.h"
#include "board.h"
#include "king.h"
#include "knight.h"
#include "pawn.h"
#include "piece.h"
#include "player.h"
#include "queen.h"
#include "rook.h"
#include "square.h"

Game::Game() = default;

Game::~Game() = default;

/**
 * Initialize the board by putting the pieces in the correct locations
 */
void Game::initialize()
{
  // Set the owner to null since they haven't been initialized yet. We'll add
  // them later
  King* black_king = new King(*_player1, Color::black, (Board::get_board().square_at(4, 7)));
  King* white_king = new King(*_player2, Color::white, (Board::get_board().square_at(4, 0)));

  // Create the players
  _player1 = new Player("White", *white_king);
  _player2 = new Player("Black", *black_king);

  white_king->set_owner(*_player1);
  black_king->set_owner(*_player2);

  auto& board = Board::get_board();
  board.square_at(0, 0).set_occupier(
      *(new Rook(*_player1, Color::white, (board.square_at(0, 0)))));
  board.square_at(1, 0).set_occupier(
      *(new Knight(*_player1, Color::white, (board.square_at(1, 0)))));
  board.square_at(2, 0).set_occupier(
      *(new Bishop(*_player1, Color::white, (board.square_at(2, 0)))));
  board.square_at(3, 0).set_occupier(
      *(new Queen(*_player1, Color::white, (board.square_at(3, 0)))));
  board.square_at(4, 0).set_occupier(*white_king);
  board.square_at(5, 0).set_occupier(
      *(new Bishop(*_player1, Color::white, (board.square_at(5, 0)))));
  board.square_at(6, 0).set_occupier(
      *(new Knight(*_player1, Color::white, (board.square_at(6, 0)))));
  board.square_at(7, 0).set_occupier(
      *(new Rook(*_player1, Color::white, (board.square_at(7, 0)))));
  board.square_at(0, 1).set_occupier(
      *(new Pawn(*_player1, Color::white, (board.square_at(0, 1)))));
  board.square_at(1, 1).set_occupier(
      *(new Pawn(*_player1, Color::white, (board.square_at(1, 1)))));
  board.square_at(2, 1).set_occupier(
      *(new Pawn(*_player1, Color::white, (board.square_at(2, 1)))));
  board.square_at(3, 1).set_occupier(
      *(new Pawn(*_player1, Color::white, (board.square_at(3, 1)))));
  board.square_at(4, 1).set_occupier(
      *(new Pawn(*_player1, Color::white, (board.square_at(4, 1)))));
  board.square_at(5, 1).set_occupier(
      *(new Pawn(*_player1, Color::white, (board.square_at(5, 1)))));
  board.square_at(6, 1).set_occupier(
      *(new Pawn(*_player1, Color::white, (board.square_at(6, 1)))));
  board.square_at(7, 1).set_occupier(
      *(new Pawn(*_player1, Color::white, (board.square_at(7, 1)))));

  // Round two!
  board.square_at(0, 7).set_occupier(
      *(new Rook(*_player2, Color::black, (board.square_at(0, 7)))));
  board.square_at(1, 7).set_occupier(
      *(new Knight(*_player2, Color::black, (board.square_at(1, 7)))));
  board.square_at(2, 7).set_occupier(
      *(new Bishop(*_player2, Color::black, (board.square_at(2, 7)))));
  board.square_at(3, 7).set_occupier(
      *(new Queen(*_player2, Color::black, (board.square_at(3, 7)))));
  board.square_at(4, 7).set_occupier(*black_king);
  board.square_at(5, 7).set_occupier(
      *(new Bishop(*_player2, Color::black, (board.square_at(5, 7)))));
  board.square_at(6, 7).set_occupier(
      *(new Knight(*_player2, Color::black, (board.square_at(6, 7)))));
  board.square_at(7, 7).set_occupier(
      *(new Rook(*_player2, Color::black, (board.square_at(7, 7)))));
  board.square_at(0, 6).set_occupier(
      *(new Pawn(*_player2, Color::black, (board.square_at(0, 6)))));
  board.square_at(1, 6).set_occupier(
      *(new Pawn(*_player2, Color::black, (board.square_at(1, 6)))));
  board.square_at(2, 6).set_occupier(
      *(new Pawn(*_player2, Color::black, (board.square_at(2, 6)))));
  board.square_at(3, 6).set_occupier(
      *(new Pawn(*_player2, Color::black, (board.square_at(3, 6)))));
  board.square_at(4, 6).set_occupier(
      *(new Pawn(*_player2, Color::black, (board.square_at(4, 6)))));
  board.square_at(5, 6).set_occupier(
      *(new Pawn(*_player2, Color::black, (board.square_at(5, 6)))));
  board.square_at(6, 6).set_occupier(
      *(new Pawn(*_player2, Color::black, (board.square_at(6, 6)))));
  board.square_at(7, 6).set_occupier(
      *(new Pawn(*_player2, Color::black, (board.square_at(7, 6)))));

  // Add the pieces on rows 0 and 1 to the white players collection, and the
  // pieces on rows 6 and 7 to the black players pieces collection.
  for (int i = 0; i < 8; i++)
  {
    _player1->my_pieces().insert(&Board::get_board().square_at(i, 0).occupied_by());
    _player1->my_pieces().insert(&Board::get_board().square_at(i, 1).occupied_by());
    _player2->my_pieces().insert(&Board::get_board().square_at(i, 6).occupied_by());
    _player2->my_pieces().insert(&Board::get_board().square_at(i, 7).occupied_by());
  }
}

Player& Game::get_next_player()
{
  if (!_currentPlayer)
  {
    // Return player1 the first time this is called
    _currentPlayer = _player1;
  }
  else
  {
    _currentPlayer = &(opponent_of(*_currentPlayer));
  }

  return *_currentPlayer;
}

Player& Game::opponent_of(Player const& player)
{
  Player* result = _player1;

  // Make sure to return the opposite of the player who was passed in.
  if (&player == _player1)
  {
    result = _player2;
  }

  return *result;
}
