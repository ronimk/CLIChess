#pragma once
#include "Square.h"

// The Board class contains the state of the chess board at each given turn.
// The board consists of an filleLim x rankLim (defined in CLIChessDefinitions.h) array
// of Squares and a set of interface methods to access and manipulate each of the
// squares.

class Board
{
private:
	Square squares[fileLim][rankLim];
	
	void validateSquare(const SquareCoords& coords) const;

public:
	const Square& getSquare(const SquareCoords& coords) const;
	void emptyBoard();
	void removePiece(const SquareCoords& coords);
	void removePiece(const std::shared_ptr<Piece> _piece);
	void setPiece(const std::shared_ptr<Piece> _piece);

	bool hasPiece(const SquareCoords& coords) const;
	const Player* squareOwner(const SquareCoords& coords) const;
	MoveId squarePieceType(const SquareCoords& coords) const;
	std::shared_ptr<Piece> getPiece(const SquareCoords& coords) const;
};