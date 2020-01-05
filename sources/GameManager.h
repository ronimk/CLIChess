#pragma once
#include <string>
#include "CLIChessDefinitions.h"
#include "CLIChessExceptions.h"
#include "MoveParser.h"
#include "Board.h"
#include "Player.h"
#include "Square.h"
#include "Pieces.h"

// GameManager:
// The centralized logic class for the program.
// Everything from starting the game and making a chess move to printing
// the board to the players is handled through the GameManager object.
class GameManager
{
private:
	// Special definitions for pawn moves, used to identify to which direction
    // the players' pawns are moving:
	static const int blackRankDir =  1;
	static const int whiteRankDir = -1;
	// moveLinePadding constants is used to make the move line printing look nicer:
	static const int playerPadding = 10;

	Player white;
	Player black;
	Player *inTurn;
	Board board;
	MoveParser mParser;
	std::vector<std::string> moves;

	PieceMapper pieceMapper;

	std::string lastMsg;
	std::string whiteName;
	std::string blackName;

	int turnNum;
	int lastCapture;
	bool checkmate;
	bool stalemate;

	void initGame();
	void initNewPiece(std::shared_ptr<Piece> np, Player* owner);
	void changeTurn();
	Player* getOpponent(Player *p);
	int getOpponentDirection(Player *p);
	bool matchSrcSquare(std::shared_ptr<Piece> p, SquareCoords& coords);
	void extractMove(MoveAnalysisResults& results);
	void printMoveLine(std::ostream& out, char separator, int padding, int lineNum) const;
	void handlePromotion(MoveAnalysisResults& results);
	bool handleCastling(MoveId mP, bool test=false);
	bool threatensSquare(const SquareCoords& dest, Player* player);
	bool canMove(Player* player);
	void finalizeGameState(MoveAnalysisResults& results);
	bool validateMove(const MoveAnalysisResults& results, Player* player);
	void commitMove(MoveAnalysisResults& results);

public:
	GameManager();
	bool makeMove(std::string move);
	const std::string& getMsg() const;
	bool isCheckmate() const;
	bool isStalemate() const;
	void printBoard() const;
	std::string inTurnPlayer() const;
	void restart();
	bool save(std::string filename);
	bool load(std::string filename);
	bool takeBack(size_t n);
};

