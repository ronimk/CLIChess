#include "MoveParser.h"
#include "CLIChessDefinitions.h"
#include "CLIChessExceptions.h"

// Private methods:
// ----------------
int MoveParser::fileValue(char file)
{
	return file - 'a';
}

int MoveParser::rankValue(char rank)
{
	return rank - '1';
}

// stripSpecialNotation: std::string -> void
// Modifies the given move string by stripping it of special
// En Passant, promotion, check and checkmate notations.
//
// In effect, stripSpecialNotation undoes what addSpecialNotation
// does to a move.
void MoveParser::stripSpecialNotation(std::string& move) {
	size_t found = move.find(enPassantStr);

	if (found != std::string::npos) {
		move.erase(found, std::string::npos);
	}
	else {
		size_t len = move.size();
		for (found = 0; found < len; found++)
			if (move[found] == checkSymbol || move[found] == promotionSymbol || move[found] == checkmateSymbol)
				break;

		if (found < len)
			move.erase(found, std::string::npos);
	}
}

// Public methods:
// ---------------

// ParseNewMove:
// Parses the given chess move (given in algebraic notation) and extracts all the information of the move.
//
// TODO: change the parser so that it is built around a suitable regexp recognizer.
void MoveParser::parseNewMove(MoveAnalysisResults& results, PieceMapper& pieceMapper) {
	std::string& move = results.move;

	// Check for Castling moves:
	if (!move.compare(shortCastling)) {
		results.movedP = MoveId::OO;
		return;
	}

	if (!move.compare(longCastling)) {
		results.movedP = MoveId::OOO;
		return;
	}

	// Check for a promotion move:
	// (Promotion moves are maainly used by the game loader)
	size_t promo = move.find(promotionSymbol);
	if (promo != std::string::npos)
		if (pieceMapper.find(move[promo+1]) == pieceMapper.end())
			throw ParseException("[" + move + "]: Illegal promotion piece.");
		else 
			results.promotionPiece = move[promo + 1];

	// Before processing further, sptrip the move off of any special notations:
	stripSpecialNotation(move);

	int i = 1;
	int len = move.length();

	if (len < 2)
		throw ParseException("[" + move + "]: syntax error");

	// The parser goes through the move string in normal element order.
	// However, due to the dynamic nature of the algebraic notation, the
	// data-flow can not be statically defined. Therefore i is necessary
	// for indexing the move string.

	// 1. Extract the desired piece type which will be moved:
	char p = move[0];
	if (pieceMapper.find(p) == pieceMapper.end())
		throw ParseException("[" + move + "]: illegal piece symbol");
	else
		results.movedP = pieceMapper[p];

	// 2. For practical reasons, handle a pawn move separately:
	if (results.movedP == MoveId::P) {
		if (move[1] == captureSymbol && len == 4) {
			results.capt = true;
			results.src.setCoords(fileValue(p), -1);
			results.dest.setCoords(fileValue(move[2]), rankValue(move[3]));
		}
		else if (len == 2) {
			results.src.setCoords(fileValue(move[0]), -1);
			results.dest.setCoords(fileValue(move[0]), rankValue(move[1]));
		}
		else
			throw ParseException("[" + move + "]: Bad syntax (probably in destination square)");

		if (!legalSquare_p(results.dest))
			throw ParseException("[" + move + "]: Bad syntax in the destination square.");

		return;
	}


	// 3. Check and resolve piece conflicts for identical pieces on same rank or file:
	if (files.find(move[i]) != std::string::npos && len > 3) {
		if (results.movedP == MoveId::K || results.movedP == MoveId::P) {
			throw ParseException("[" + move + "]: illegal source square specifier");
		}

		results.src.setCoords(fileValue(move[i]), -1);
		i++;
	}
	else if (ranks.find(move[i]) != std::string::npos && len > 3) {
		if (results.movedP == MoveId::K || results.movedP == MoveId::P) {
			throw ParseException("[" + move + "]: illegal source square specifier");
		}
		results.src.setCoords(-1, rankValue(move[i]));
		i++;
	}
	
	// 4. Check whether the move is a capture move or not:
	if (move[i] == captureSymbol) {
		results.capt = true;
		i++;
	}
	else results.capt = false;

	// 5. Check the destination square:
	if (len - i == 2) {
		results.dest.setCoords(fileValue(move[len-2]), rankValue(move[len-1]));

		if (!legalSquare_p(results.dest))
		{
			throw ParseException("[" + move + "]: Bad syntax in the destination square.");
		}
	}
	else
		throw ParseException("[" + move + "]: Bad syntax (probably in destination square)");
}

// addSpecialNotation: results -> void
// Modifies the move string of the results DS if necessary,
// according to whether the move was an En Passant, a promotion,
// a check or a checkmate.
void MoveParser::addSpecialNotation(MoveAnalysisResults& results) {
	if (results.enPassantMove)
		results.move += enPassantStr;

	if (results.promotionMove) {
		results.move += promotionSymbol;
		results.move += results.promotionPiece;
	}

	if (results.checkMove)
		results.move += checkSymbol;

	if (results.checkmateMove)
		results.move += checkmateSymbol;
}