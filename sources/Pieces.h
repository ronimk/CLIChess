#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Board.h"
#include "CLIChessDefinitions.h"

class Player;

// Piece is an abstract class that defines the basic data structure and interface
// for every individual chess piece in the game.
//
// The most important interface method is the canMoveTo, which checks if a piece is
// allowed to move from its current position to the desired destination position
// according to the rules of chess.
class Piece {
protected:
	SquareCoords coords;

	// hasMoved is used for implementing the "double square", the en passant and the castling special moves:
	bool hasMoved;

	// In order to make it easy for the system to figure out which player any individual piece belongs to,
	// every piece has a pointer to the player it belongs to:
	const Player* player;

	bool canFollowStraightLine(const SquareCoords& destCoords, const Board& board) const;

public:
	Piece(const SquareCoords& _coords, const Player* _player) : coords(_coords) { hasMoved = false; player = _player; }
	virtual ~Piece() {}
	virtual const Player* getOwner() const { return player; }
	virtual MoveId getType() const = 0;		// The piece's type is determined by the concrete piece class it is modeled on.
	const SquareCoords& getCoords() const { return coords; }
	void setCoords(const SquareCoords& newCoords) { coords = newCoords; }
	bool moved_p() const { return hasMoved; }
	virtual bool canMoveTo(MoveAnalysisResults& results, const Board& board) const;
	virtual void reachableSquares(std::vector<SquareCoords>& sqList, int oppDir, const Board& board) const;
	virtual bool threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const = 0;
	virtual bool canBeEnPassanted() const { return false; }
	virtual void setEnPassant() { }
	virtual void reset() { } 
	void move() { hasMoved = true; }
	friend std::ostream& operator<<(std::ostream& out, const Piece& p);
	virtual std::string toString() const = 0;
	
	// NOTE:
	// The reset method is currently used for resetting EnPassant flag for pawns, but could be used for creating
	// more interesting chess variants with different pieces having effects lasting even multiple turns.
};

// A pawn has the type MoveId::P and a string representation "P".
class Pawn : public Piece {
private:
	bool enPassantThreat;
	bool canCapture(MoveAnalysisResults& results, int opponentDir, const Board& board) const;
public:
	Pawn(const SquareCoords& _coords, const Player* _player) : Piece(_coords, _player) { enPassantThreat = false; }
	virtual MoveId getType() const { return MoveId::P; }
	virtual bool canMoveTo(MoveAnalysisResults& results, const Board& board) const;
	virtual void reachableSquares(std::vector<SquareCoords>& sqList, int oppDir, const Board& board) const;
	virtual bool threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const;
	virtual bool canBeEnPassanted() const { return enPassantThreat; }
	virtual void setEnPassant() { enPassantThreat = true; }
	virtual void reset() { enPassantThreat = false; }
	virtual std::string toString() const { return "P"; }
};

// A pawn has the type MoveId::R and a string representation "R".
class Rook : public Piece {
public:
	Rook(const SquareCoords& _coords, const Player* _player) : Piece(_coords, _player) {}
	virtual MoveId getType() const { return MoveId::R; }
	virtual bool threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const;
	virtual std::string toString() const { return "R"; }
};

// A pawn has the type MoveId::N and a string representation "N".
class Knight : public Piece {
public:
	Knight(const SquareCoords& _coords, const Player* _player) : Piece(_coords, _player) {}
	virtual MoveId getType() const { return MoveId::N; }
	virtual bool threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const;
	virtual std::string toString() const { return "N"; }
};

// A pawn has the type MoveId::B and a string representation "B".
class Bishop : public Piece {
public:
	Bishop(const SquareCoords& _coords, const Player* _player) : Piece(_coords, _player) {}
	virtual MoveId getType() const { return MoveId::B; }
	virtual bool threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const;
	virtual std::string toString() const { return "B"; }
};

// A pawn has the type MoveId::Q and a string representation "Q".
class Queen : public Piece {
public:
	Queen(const SquareCoords& _coords, const Player* _player) : Piece(_coords, _player) {}
	virtual MoveId getType() const { return MoveId::Q; }
	virtual bool threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const;
	virtual std::string toString() const { return "Q"; }
};

// A pawn has the type MoveId::K and a string representation "K".
class King : public Piece {
public:
	King(const SquareCoords& _coords, const Player* _player) : Piece(_coords, _player) {}
	virtual MoveId getType() const { return MoveId::K; }
	virtual bool threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const;
	virtual std::string toString() const { return "K"; }
};