#include "Pieces.h"
#include "CLIChessExceptions.h"

std::ostream& operator<<(std::ostream& out, const Piece& p) {
	out << p.toString();
	return out;
}

// moveDirection: AxisSrc, AxisDest -> AxisDirection
// Is used to check into which direction a piece is trying to move along a given axis.
int moveDirection(int src, int dest) {
	if (src > dest)
		return -1;
	else if (src < dest)
		return 1;
	else
		return 0;
}

// canFollowStraightLine:
// A helper function for checking whether rooks, bishops and queens can follow a straight line from their square to
// the given destination square without bumping into any other piece, having to turn at any point or going over the board:
bool Piece::canFollowStraightLine(const SquareCoords& destCoords, const Board& board) const {
	// 1. Extract move direction:
	int destFile = destCoords.file();
	int destRank = destCoords.rank();
	int fileDir = moveDirection(coords.file(), destFile);
	int rankDir = moveDirection(coords.rank(), destRank);

	try {
		for (int moveFile = coords.file() + fileDir, moveRank = coords.rank() + rankDir; ; moveFile += fileDir, moveRank += rankDir) {
			if (destFile == moveFile && destRank == moveRank)
				return true;
			else if (board.hasPiece(SquareCoords(moveFile, moveRank))) return false;
		}
	}
	catch (BoardAccessException const& e) {
		return false;
		// There was no straight line of adjacent squares without turns
		// between the piece's source square and the desired destination square.
	}
}

// The following move validators might be best implemented with some form of state machines instead of their current implementations.
// They all require the following MoveAnalysisResults-data to be set beforehand:
//
//		src
//		dest
//		capt
//		opponentDir
//
// NOTE:
// even though the following functions heavily use results, which has a pure public access to every member (for a convenience reason),
// they only modify the results ds when the move is guaranteed to be possible to make. Furthermore, if there is any move ambiquity,
// the player didn't use the correct notation and the entire move analysis should be abandoned with the results invalidated.
// Therefore the same MoveAnalysisResults are safe to be used for multiple different piéces to check whether any of them can make the given move.
//
// Also, the pawn moves in a highly specialized and asymmetric fashion, therefor it has its own canMoveTo-method.
bool Piece::canMoveTo(MoveAnalysisResults& results, const Board& board) const {
	// Check to see if the given piece can reach the square:
	if (threatensSquare(results.dest, results.opponentDir, board)) {
		// 3. Check whether the destination square meets all the required conditions:
		//  I. that it has an opponent's piece and the move is a capture move, OR
		// II. the square is free and the move is NOT a capture move
		if (board.hasPiece(results.dest))
			if (results.capt && board.squareOwner(results.dest) != player) {
				results.captureCoords = results.dest;
				return true;
			}
			else return false;
		else return !results.capt;
	}
	else return false;
}

// reachableSquares: std::vector<SquareCoords>&, int, Board -> void
// Finds all the reachable squares for the piece and pushes them into the given sqList.
// NOTE:
// This is not the most efficient solution by any means, but it perfectly reuses code
// already implemented. In a two player chess it should not cause any bottlenecks, but
// for a full-fledged computer chess engine, this would obviously be an inefficient solution.
//
// Note: Pawn moves in a highly specialized and asymmetric fasion,
// therefore it has its own implementation of reachableSquares.
void Piece::reachableSquares(std::vector<SquareCoords>& sqList, int oppDir, const Board& board) const {
	for (int file=0; file<fileLim; file++)
		for (int rank=0; rank<rankLim; rank++) {
			SquareCoords next(file, rank);
			if (threatensSquare(next, oppDir, board) &&
				(!board.hasPiece(next) || board.squareOwner(next) != player) )
				sqList.push_back(next);
		}
}

bool Pawn::canCapture(MoveAnalysisResults& results, int opponentDir, const Board& board) const {
	int srcFile = coords.file();
	int srcRank = coords.rank();
	int destFile = results.dest.file();
	int destRank = results.dest.rank();
	SquareCoords epSq(destFile, srcRank);

	// 1. Check if the destFile and destRank match
	//    the space in which a pawn can capture:
	int fileDir = moveDirection(srcFile, destFile);
	int rankDir = moveDirection(srcRank, destRank);

	if (fileDir == 0 || destFile != srcFile + fileDir || rankDir != opponentDir || destRank != srcRank + rankDir)
		return false;

	// 2. check for normal capture:
	if (board.hasPiece(results.dest) && board.squareOwner(results.dest) != player) {
		results.captureCoords = results.dest;
		if (destRank == 0 || destRank == rankLim - 1)
			results.promotionMove = true;
		return true;
	}
	// 3. check for En Passant:
	else if (board.hasPiece(epSq) && board.squareOwner(epSq) != player &&
		board.getPiece(SquareCoords(epSq))->canBeEnPassanted()) {
		results.captureCoords = epSq;
		results.enPassantMove = true;
		return true;
	}
	else return false;
}

bool Pawn::canMoveTo(MoveAnalysisResults& results, const Board& board) const {
	// For pawns, the move depends on three factors:
	//   I. whether the move is a capture move or not,
	//  II. whether the pawn has already moved or not (dependent on whether capture or not)
	// III. whether the pawn being captured has already moved or not (dependent on whether capture or not)
	// Thus, the move logic is divided into two main branches:

	if (results.capt)
		// 1. if the move is a capture move, check for the possibility of a capture:
		return canCapture(results, results.opponentDir, board);
	else {
		// 2. The move is not a capture move:
		int srcRank = coords.rank();
		int destRank = results.dest.rank();

		if (board.hasPiece(results.dest))
			return false;

		// 2.A) check if the destFile and destRank match
		//      the space in which a pawn can move:
		int rankDir = moveDirection(srcRank, destRank);
		if (!coords.sameFile(results.dest) || rankDir != results.opponentDir)
			return false;

		// Check if the pawn can be moved to the destination square:
		if (destRank == srcRank + rankDir) {
			if (destRank == 0 || destRank == rankLim - 1)
				results.promotionMove = true;
			return true;
		}
		else if (destRank == srcRank + rankDir * 2 && hasMoved == false && !board.hasPiece(SquareCoords(results.dest.file(), srcRank + rankDir))) {
			results.enPassantThreat = true;
			return true;
		}
		else return false;
	}
}

void Pawn::reachableSquares(std::vector<SquareCoords>& sqList, int oppDir, const Board& board) const {
	SquareCoords next(coords.file(), coords.rank() + oppDir);

	// check non-capture squares:
	if (legalSquare_p(next) && !board.hasPiece(next)) {
		sqList.push_back(next);
		if (!hasMoved) {
			next.setCoords(next.file(), next.rank() + oppDir);
			if (legalSquare_p(next) && !board.hasPiece(next))
				sqList.push_back(next);
		}
	}
	
	// check for "left-side" capture squares:
	next.setCoords(coords.file() - 1, coords.rank() + oppDir);
	SquareCoords epSq(coords.file() - 1, coords.rank());

	if (legalSquare_p(next)) {
		if (board.hasPiece(next) && board.squareOwner(next) != player)
			sqList.push_back(next);
		else if (!board.hasPiece(next)) {
			if (board.hasPiece(epSq) && board.squareOwner(epSq) != player) {
				std::shared_ptr<Piece> opp = board.getPiece(epSq);
				if (opp->getType() == MoveId::P && !opp->moved_p())
					sqList.push_back(next);
			}
		}
	}

	// Check for "right-side" capture square:
	next.setCoords(coords.file() + 1, coords.rank() + oppDir);
	epSq.setCoords(coords.file() + 1, coords.rank());

	if (legalSquare_p(next)) {
		if (board.hasPiece(next) && board.squareOwner(next) != player)
			sqList.push_back(next);
		else if (!board.hasPiece(next)) {
			if (board.hasPiece(epSq) && board.squareOwner(epSq) != player) {
				std::shared_ptr<Piece> opp = board.getPiece(epSq);
				if (opp->getType() == MoveId::P && !opp->moved_p())
					sqList.push_back(next);
			}
		}
	}
}

bool Pawn::threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const {
	return destCoords.rank() == coords.rank() + opponentDir &&
		  (destCoords.file() == coords.file() + 1 || destCoords.file() == coords.file() - 1);
}

bool Rook::threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const {
	if (destCoords == coords || (!coords.sameFile(destCoords) && !coords.sameRank(destCoords)))
		return false;

	return canFollowStraightLine(destCoords, board);
}

bool Knight::threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const {
	if (!legalSquare_p(destCoords))
		return false;
	int srcFile = coords.file();
	int srcRank = coords.rank();
	int destFile = destCoords.file();
	int destRank = destCoords.rank();

	if (!((destFile == srcFile + 2 && destRank == srcRank + 1) ||
		  (destFile == srcFile + 2 && destRank == srcRank - 1) ||
		  (destFile == srcFile - 2 && destRank == srcRank + 1) ||
		  (destFile == srcFile - 2 && destRank == srcRank - 1) ||
		  (destRank == srcRank + 2 && destFile == srcFile + 1) ||
		  (destRank == srcRank + 2 && destFile == srcFile - 1) ||
		  (destRank == srcRank - 2 && destFile == srcFile + 1) ||
		  (destRank == srcRank - 2 && destFile == srcFile - 1)))
		return false;
	else return true;
}

bool Bishop::threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const {
	if (coords.sameFile(destCoords) || coords.sameRank(destCoords))
		return false;

	return canFollowStraightLine(destCoords, board);
}

bool Queen::threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const {
	if (destCoords == coords)
		return false;

	return canFollowStraightLine(destCoords, board);
}

bool King::threatensSquare(const SquareCoords& destCoords, int opponentDir, const Board& board) const {
	int fileDir = moveDirection(coords.file(), destCoords.file());
	int rankDir = moveDirection(coords.rank(), destCoords.rank());

	return (destCoords != coords      &&
		destCoords.file() == coords.file() + fileDir &&
		destCoords.rank() == coords.rank() + rankDir);
}