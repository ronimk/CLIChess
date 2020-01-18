#pragma once
#include <string>
#include <map>

// all of the above datastructures and constants seemed generic enough
// and/or so widely used as to not warrant encapsulation inside specific classes.

// MoveId is used to identify the type of the move
enum class MoveId { NaP, P, R, N, B, Q, K, OO, OOO };
using PieceId = MoveId;
// For the special piece moves:
// NaP: Not a Move, meaning an illegal move
// OO:  Short Castling
// OOO: Long Castling

// The colors below are used by CLIChess.cpp and the GameManager's board printer:
#define whitePieceColor 0x4F
#define blackPieceColor 0x1F
#define consoleColor 0x07

// The chessboard file and rank sizes. If desired, these can be changed
// to create a chess variant with a bigger/smaller board.
// Everything else except the castling routine and the game initializer
// should adjust to changing the board size limits:
const int fileLim = 8;
const int rankLim = 8;

// SquareCoords is used for identifying a square on the game board.
typedef std::pair<int, int> coords;

class SquareCoords {
private:
	coords pos;

public:
	SquareCoords();
	SquareCoords(int file, int rank);
	SquareCoords(const SquareCoords& copy);
	SquareCoords& operator=(const SquareCoords& rhs);
	bool operator==(const SquareCoords& rhs) const;
	bool operator!=(const SquareCoords& rhs) const;
	bool sameFile(const SquareCoords& comp) const;
	bool sameRank(const SquareCoords& comp) const;
	int file() const;
	int rank() const;
	void setCoords(int file, int rank);
	friend std::ostream& operator<<(std::ostream& out, const SquareCoords& sc);
};


// When analyzing that a piece can take a certain move,
// an object of MoveAnalysisResult is used in order
// to make it as easy as possible for the gameManager
// to extract all the necessary information about the
// given move.
class MoveAnalysisResults {
public:
	std::string move;
	MoveId movedP;				
	SquareCoords src;			// Required to be set before calling a piece's canMoveTo-method.
	SquareCoords dest;			// Required to be set before calling a piece's canMoveTo-method.
	bool capt;					// Required to be set before calling a piece's canMoveTo-method.
	int opponentDir;			// Is the absolute direction on the game board of the opponent's side: is needed for pawn moves.
								// opponentDir is also required to be set before calling a piece's canMoveTo-method.

	// The following member variables will be set by a piece's canMoveTo-method if the move conatined within can be made:
	bool enPassantThreat;		// Is set when a pawn moves two squares forward on its first move.
	bool enPassantMove;			// Is set when a pawn would En Passant an opponent's pawn.
	bool promotionMove;			// Is set when a pawn is being promoted
	SquareCoords captureCoords;
	// For En Passant, the captured pawn will reside on different square
	// than the capturing pawn's destination square, thus this field is
	// required to identify the square of the captured piece.

	// The following member variables will be set by the GameManager when it checks the final state of the board after a move has been made:
	bool checkMove;				// Is set when a move is a check
	bool checkmateMove;			// Is set when a move is a checkmate
	std::string promotionPiece;	// Is set to the promoted piece's textual representation when a pawn is promoted

	MoveAnalysisResults() { movedP = MoveId::NaP; capt = false; opponentDir = 0; enPassantThreat = false; enPassantMove = false; promotionMove = false; checkMove = false; checkmateMove = false; }
};

// PieceMapper is used by GameManager and MoveParser to search for
// the MoveId of a user-typed char representation of a piece.
// The GameManager uses it to promote a pawn, while the MoveParser
// uses it to identify the type of piece that the player in turn
// desires to move.
class PieceMapper {
private:
	std::map<char, MoveId> pieceLkupTable;

public:
	std::map<char, MoveId>::iterator begin() noexcept { return pieceLkupTable.begin(); }
	std::map<char, MoveId>::iterator end() noexcept { return pieceLkupTable.end(); }
	std::map<char, MoveId>::iterator find(const char& Keyval) { return pieceLkupTable.find(Keyval); }
	MoveId& operator[] (const char& k) { return pieceLkupTable[k]; }
};

// legalSquare_p: const squareCoords& -> bool
// A helper predicate function for ensuring that the given
// coordinates point to a valid square in the board.
bool legalSquare_p(const SquareCoords& coords);