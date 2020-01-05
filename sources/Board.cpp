#include <iostream>
#include "Board.h"
#include "Pieces.h"
#include "CLIChessExceptions.h"
#include "CLIChessDefinitions.h"

// validateSquare: const squareCoords& -> void
// validates that the given coordinates are within the game area.
// If they are not, throws a BoardAccessException.
void Board::validateSquare(const SquareCoords& coords) const {
	if (!legalSquare_p(coords))
		throw BoardAccessException(std::string("Trying to access an invalid square. Given indexes (file, rank): (") + std::to_string(coords.file()) + ", " + std::to_string(coords.rank()) + ").");
}

// getSquare: const squareCoords& -> const Square&
// Returns a constant reference to the square at the given coordinates.
const Square& Board::getSquare(const SquareCoords& coords) const {
	validateSquare(coords);
	return squares[coords.file()][coords.rank()];
}

// emptyBoard: void -> void
// Empties the game board.
// Is used for both initializing the game and restarting it.
void Board::emptyBoard() {
	for (int i = 0; i < fileLim; i++)
		for (int j = 0; j < rankLim; j++)
			squares[i][j].removePiece();
}

// removePiece: const squareCoords& -> void
// Removes a piece from the square at the given coordinates.
void Board::removePiece(const SquareCoords& coords) {
	validateSquare(coords);
	squares[coords.file()][coords.rank()].removePiece();
}

// removePiece: const std::shared_ptr<Piece> -> void
// Removes a piece from the game board.
//
// NOTE:
// It is the responsibility of the GameManager to ensure that
// the given piece really is attached to its square on the board.
void Board::removePiece(const std::shared_ptr<Piece> _piece) {
	removePiece(_piece->getCoords());
}

// setPiece: const std::shared_ptr<Piece> -> void
// Sets the Square at the given piece's coordinates to point
// to the given piece.
void Board::setPiece(const std::shared_ptr<Piece> _piece) {
	SquareCoords coords = _piece->getCoords();
	validateSquare(coords);
	squares[coords.file()][coords.rank()].setPiece(_piece);

}

// hasPiece: const squareCoords& -> bool
// Returns true if the square at the given coordinates
// contains a piece, otherwise returns false.
bool Board::hasPiece(const SquareCoords& coords) const {
	validateSquare(coords);
	return squares[coords.file()][coords.rank()].hasPiece();
}

// squareOwner: const squareCoords& -> const ptr to Player
// Returns a pointer to the player who owns the piece on the
// square at the given coordinates.
//
// NOTE:
//  calling this method by itself may not be safe.
//  One should always first check if a piece exists
//  in the square by calling the board's hasPiece
//  -method.
//
//  ex. if (board.hasPiece(coords))
//		    const Player* owner = board.pieceOwner(coords);
//
const Player* Board::squareOwner(const SquareCoords& coords) const {
	validateSquare(coords);
		return squares[coords.file()][coords.rank()].pieceOwner();
}

// squarePieceType: const squareCoords& -> MoveId
// retuns the type of the piece on the square at
// the given coordinates.
//
// NOTE:
//  calling this method by itself may not be safe.
//  One should always first check if a piece exists
//  in the square by calling the board's hasPiece
//  -method.
//
//  ex. if (board.hasPiece(coords))
//		    MoveId type = board.squarePieceType(coords);
//
MoveId Board::squarePieceType(const SquareCoords& coords) const {
	validateSquare(coords);
	return squares[coords.file()][coords.rank()].pieceType();
}

// getPiece: const squareCoords& -> std::shared_ptr<Piece>
// retuns a pointer to the piece on the square 
// at the given coordinates.
//
// NOTE:
//  calling this method by itself may not be safe.
//  One should always first check if a piece exists
//  in the square by calling the board's hasPiece
//  -method.
//
//  ex. if (board.hasPiece(coords))
//		    std::shared_ptr<Piece> p = board.getPiece(coords);
//
std::shared_ptr<Piece> Board::getPiece(const SquareCoords& coords) const {
	validateSquare(coords);
		return squares[coords.file()][coords.rank()].getPiece();
}