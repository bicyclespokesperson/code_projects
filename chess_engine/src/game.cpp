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

// These will be initialized in the initialize method
Player* Game::_player1 = NULL;
Player* Game::_player2 = NULL;
Player* Game::_currentPlayer = _player1;

Game::Game() = default;

Game::~Game() = default;

Player& Game::get_next_player()
{
  // Since current player is initialized at null, this will return
  // player one for the first move.
  _currentPlayer = &(opponent_of(*_currentPlayer));

  return *_currentPlayer;
}

/**
 * Initialize the board by putting the pieces in the correct locations
 */
void Game::initialize()
{
  // Set the owner to null since they haven't been initialized yet. We'll add
  // them later
  King* black_king = new King(*_player1, "black", (Board::get_board().square_at(4, 7)));
  King* white_king = new King(*_player2, "white", (Board::get_board().square_at(4, 0)));

  // Create the players
  _player1 = new Player("white", *white_king, *(new std::set<Piece*>));
  _player2 = new Player("black", *black_king, *(new std::set<Piece*>));

  white_king->set_owner(*_player1);
  black_king->set_owner(*_player2);

  // This here be beautiful code!
  Board::get_board().square_at(0, 0).set_occupier(
      *(new Rook(*_player1, "white", (Board::get_board().square_at(0, 0)))));
  Board::get_board().square_at(1, 0).set_occupier(
      *(new Knight(*_player1, "white", (Board::get_board().square_at(1, 0)))));
  Board::get_board().square_at(2, 0).set_occupier(
      *(new Bishop(*_player1, "white", (Board::get_board().square_at(2, 0)))));
  Board::get_board().square_at(3, 0).set_occupier(
      *(new Queen(*_player1, "white", (Board::get_board().square_at(3, 0)))));
  Board::get_board().square_at(4, 0).set_occupier(*white_king);
  Board::get_board().square_at(5, 0).set_occupier(
      *(new Bishop(*_player1, "white", (Board::get_board().square_at(5, 0)))));
  Board::get_board().square_at(6, 0).set_occupier(
      *(new Knight(*_player1, "white", (Board::get_board().square_at(6, 0)))));
  Board::get_board().square_at(7, 0).set_occupier(
      *(new Rook(*_player1, "white", (Board::get_board().square_at(7, 0)))));
  Board::get_board().square_at(0, 1).set_occupier(
      *(new Pawn(*_player1, "white", (Board::get_board().square_at(0, 1)))));
  Board::get_board().square_at(1, 1).set_occupier(
      *(new Pawn(*_player1, "white", (Board::get_board().square_at(1, 1)))));
  Board::get_board().square_at(2, 1).set_occupier(
      *(new Pawn(*_player1, "white", (Board::get_board().square_at(2, 1)))));
  Board::get_board().square_at(3, 1).set_occupier(
      *(new Pawn(*_player1, "white", (Board::get_board().square_at(3, 1)))));
  Board::get_board().square_at(4, 1).set_occupier(
      *(new Pawn(*_player1, "white", (Board::get_board().square_at(4, 1)))));
  Board::get_board().square_at(5, 1).set_occupier(
      *(new Pawn(*_player1, "white", (Board::get_board().square_at(5, 1)))));
  Board::get_board().square_at(6, 1).set_occupier(
      *(new Pawn(*_player1, "white", (Board::get_board().square_at(6, 1)))));
  Board::get_board().square_at(7, 1).set_occupier(
      *(new Pawn(*_player1, "white", (Board::get_board().square_at(7, 1)))));

  // Round two!
  Board::get_board().square_at(0, 7).set_occupier(
      *(new Rook(*_player2, "black", (Board::get_board().square_at(0, 7)))));
  Board::get_board().square_at(1, 7).set_occupier(
      *(new Knight(*_player2, "black", (Board::get_board().square_at(1, 7)))));
  Board::get_board().square_at(2, 7).set_occupier(
      *(new Bishop(*_player2, "black", (Board::get_board().square_at(2, 7)))));
  Board::get_board().square_at(3, 7).set_occupier(
      *(new Queen(*_player2, "black", (Board::get_board().square_at(3, 7)))));
  Board::get_board().square_at(4, 7).set_occupier(*black_king);
  Board::get_board().square_at(5, 7).set_occupier(
      *(new Bishop(*_player2, "black", (Board::get_board().square_at(5, 7)))));
  Board::get_board().square_at(6, 7).set_occupier(
      *(new Knight(*_player2, "black", (Board::get_board().square_at(6, 7)))));
  Board::get_board().square_at(7, 7).set_occupier(
      *(new Rook(*_player2, "black", (Board::get_board().square_at(7, 7)))));
  Board::get_board().square_at(0, 6).set_occupier(
      *(new Pawn(*_player2, "black", (Board::get_board().square_at(0, 6)))));
  Board::get_board().square_at(1, 6).set_occupier(
      *(new Pawn(*_player2, "black", (Board::get_board().square_at(1, 6)))));
  Board::get_board().square_at(2, 6).set_occupier(
      *(new Pawn(*_player2, "black", (Board::get_board().square_at(2, 6)))));
  Board::get_board().square_at(3, 6).set_occupier(
      *(new Pawn(*_player2, "black", (Board::get_board().square_at(3, 6)))));
  Board::get_board().square_at(4, 6).set_occupier(
      *(new Pawn(*_player2, "black", (Board::get_board().square_at(4, 6)))));
  Board::get_board().square_at(5, 6).set_occupier(
      *(new Pawn(*_player2, "black", (Board::get_board().square_at(5, 6)))));
  Board::get_board().square_at(6, 6).set_occupier(
      *(new Pawn(*_player2, "black", (Board::get_board().square_at(6, 6)))));
  Board::get_board().square_at(7, 6).set_occupier(
      *(new Pawn(*_player2, "black", (Board::get_board().square_at(7, 6)))));

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

// Returns player one if argument is null
Player& Game::opponent_of(Player& player)
{
  Player* result = _player1;

  // Make sure to return the opposite of the player who was passed in.
  if (&player == _player1)
  {
    result = _player2;
  }

  return *result;
}
