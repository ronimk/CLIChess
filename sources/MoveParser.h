#pragma once
#include <iostream>
#include <map>
#include "CLIChessDefinitions.h"

// MoveParser:
// Is used for parsing the given move.
// The results of parsing are inserted into the given MoveAnalysisResults object.
class MoveParser
{
private:
	const char captureSymbol = 'x';
	const char checkmateSymbol = '#';
	const char checkSymbol = '+';
	const char promotionSymbol = '=';
	const std::string enPassantStr = "e.p.";
	const std::string shortCastling = "O-O";
	const std::string longCastling = "O-O-O";
	const std::string files = "abcdefgh";
	const std::string ranks = "12345678";

	int fileValue(char file);
	int rankValue(char rank);

	void stripSpecialNotation(std::string& move);
	bool promotionMove_p(std::string move);

public:
	void parseNewMove(MoveAnalysisResults& results, PieceMapper& pieceMapper);
	void addSpecialNotation(MoveAnalysisResults& results);
};
