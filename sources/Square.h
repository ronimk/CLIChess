#pragma once
#include <memory>
#include "CLIChessDefinitions.h"

class Player;
class Piece;

// The square class has one member variable - piece - along with
// an interface through which relevant information of the piece
// on the square can be obtained
class Square
{
private:
	std::shared_ptr<Piece> piece;

public:
	Square();
	bool hasPiece() const;
	const Player *pieceOwner() const;
	MoveId pieceType() const;
	const std::shared_ptr<Piece> getPiece() const;
	void removePiece();
	void setPiece(std::shared_ptr<Piece> _piece);
	void printPiece() const;
};

