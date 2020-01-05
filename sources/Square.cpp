#include <iostream>
#include "pieces.h"
#include "Square.h"

Square::Square()
{
	piece = nullptr;
}

// hasPiece:
// returns true if the square has a piece (or wall).
// returns false, if the square is empty.
bool Square::hasPiece() const {
	if (piece != nullptr)
		return true;
	else
		return false;
}

// pieceOwner:
// returns the owner of the piece in the square.
//
// NOTE:
//  calling this method by itself may not be safe.
//  One should always first check if a piece exists
//  in the square by calling the square's hasPiece
//  -method.
//
//  ex. if (sq.hasPiece())
//		    Player* owner = sq.pieceOwner();
//
const Player* Square::pieceOwner() const {
	return piece->getOwner();
}

// pieceType:
// returns the type of the piece in the square.
//
// NOTE:
//   calling this method by itself may not be safe.
//   One should always first check if a piece exists
//  in the square by calling the square's hasPiece
//  -method.
//
//  ex. if (sq.hasPiece())
//		    PieceID pId = sq.pieceType();
//
MoveId Square::pieceType() const
{
	return piece->getType();
}

// getPiece: void -> const std::shared_ptr<Piece>
// Returns a constant pointer to the piece at the square
// If the square has no piece, returns nullptr
const std::shared_ptr<Piece> Square::getPiece() const {
	return piece;
}

// removePiece:
// Removes any piece on the square.
//
// The function is safe to call even for empty squares.
void Square::removePiece() {
	piece = nullptr;
}

// setPiece:
// Sets the piece of the current square to the given piece
//
// The function is safe to call even for squares that already have a piece in them.
// Therefore the capturing of a piece by another piece becomes a simple call of
//     setPiece(capturingPiece);
void Square::setPiece(std::shared_ptr<Piece> _piece) {
	piece = _piece;
}

// printPiece:
// Prints the piece's textual representation in the given square.
//
// NOTE:
//   calling this method by itself may not be safe.
//   One should always first check if a piece exists
//  in the square by calling the square's hasPiece
//  -method.
//
//  ex. if (sq.hasPiece())
//		    cout << sq.printPiece();
//
void Square::printPiece() const {
	std::cout << *piece;
}