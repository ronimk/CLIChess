#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "GameManager.h"

// Private methods:
// -----------------------------

// initGame: void -> void
// Initializes the game into its initial state.
//
// First, removes any remaining pieces on the board.
// Then resets the white and black players' pieces,
// Finally, sets white player to bebgin the game
// and resets the rest of the gamestate.
void GameManager::initGame() {
	// 1. Empty all squares:
	board.emptyBoard();

	// 2. Clear white and black pieces:
	white.clear();
	black.clear();

	// 3. Set up the pawns:
	for (int file=0; file<fileLim; file++)
		initNewPiece(std::shared_ptr<Piece>(new Pawn(SquareCoords(file, 1), &white)), &white);

	for (int file = 0; file<fileLim; file++)
		initNewPiece(std::shared_ptr<Piece>(new Pawn(SquareCoords(file, 6), &black)), &black);
	
	// 4. Set up rest of the pieces:
	Player* p = &white;
	initNewPiece(std::shared_ptr<Piece>(new Rook(SquareCoords(0, 0), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Knight(SquareCoords(1, 0), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Bishop(SquareCoords(2, 0), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Queen(SquareCoords(3, 0), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new King(SquareCoords(4, 0), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Bishop(SquareCoords(5, 0), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Knight(SquareCoords(6, 0), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Rook(SquareCoords(7, 0), p)), p);
	p = &black;
	initNewPiece(std::shared_ptr<Piece>(new Rook(SquareCoords(0, 7), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Knight(SquareCoords(1, 7), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Bishop(SquareCoords(2, 7), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Queen(SquareCoords(3, 7), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new King(SquareCoords(4, 7), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Bishop(SquareCoords(5, 7), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Knight(SquareCoords(6, 7), p)), p);
	initNewPiece(std::shared_ptr<Piece>(new Rook(SquareCoords(7, 7), p)), p);

	// 5. Reset the rest of the game:
	inTurn = &white;
	turnNum = 1;
	checkmate = false;
	stalemate = false;
	moves.clear();
}

// initNewPiece: std::shared_ptr<Piece>, ptr to Player -> void
// sets up a new piece for the game.
void GameManager::initNewPiece(std::shared_ptr<Piece> np, Player* owner) {
	owner->addPiece(np);
	board.setPiece(np);
}

// changeTurn: void -> void
// changes the player in turn and prepares
// for the next game turn if necessary.
void GameManager::changeTurn() {
	// 1. Change player in turn:
	inTurn = getOpponent(inTurn);

	// 2. Increase the turn number if necessary:
	if (inTurn == &white)
		turnNum++;

	// 3. Prepare for the next player's turn:
	inTurn->beginTurn();
}

// getOpponent: ptr to Player -> ptr to Player
// Returns the opponent of the player in turn.
Player* GameManager::getOpponent(Player *p) {
	if (p == &white)
		return &black;
	else
		return &white;
}

// getOpponentDirection: ptr to Player -> int
// Returns the direction of the opponent of the given player.
// Is used for validating pawn moves.
int GameManager::getOpponentDirection(Player *p) {
	if (p == &white)
		return blackRankDir;
	else
		return whiteRankDir;
}

// matchSrcSquare: std::shared_ptr<Piece>, &squareCoords -> bool
// Tries to match the given piece to the given square coordinates.
//
// If the piece has coordinates (srcFile, srcRank), return true.
// if srcFile is -1, match for srcRank only. Likewise with srcRank being -1.
//
// if either srcFile or srcRank is set but they differ from the piece's square
// coordinates, return false.
bool GameManager::matchSrcSquare(std::shared_ptr<Piece> p, SquareCoords& coords) {
	SquareCoords pieceCoords = p->getCoords();
	if ( (coords.file() >= 0 && !pieceCoords.sameFile(coords)) ||
		 (coords.rank() >= 0 && !pieceCoords.sameRank(coords)) )
		return false;

	return true;
}

// exctractMove:
// Extracts all the necessary information for the move.
//
// Also checks for any move ambiquities:
//   if more than one piece with identical type of the current
//   player can move to the same square, and there is not enough
//   source square information to identify which of the pieces
//   should be moved, an IllegalMoveException is thrown.
//
// Also, if the move is a capture, but the player has not
// indicated this, a SquareValidationException is thrown.
// And likewise, if the move is not a capture, but the player
// has claimed it is a capture, a SquareValidationException is thrown.
// Finally, if the payer tries to move into a square occupied by
// their own piece, a SquareValidationException is thrown.
void GameManager::extractMove(MoveAnalysisResults& results) {

	// 1. Check for capture consistency:
	const Square& destSq = board.getSquare(results.dest);

	  // A Prolog model to help solve the problem:
	  // missing_capture_exception() :- hasPiece(destSq), owner(X, destSq), opponents(inTurn, X), not(capture_move()).
	  // own_square_exception() :- hasPiece(destSq), owner(inTurn, destSq).
	  // illegal_capture_exception() :- not(hasPiece(destSq)), capture_move().
	  //
	  // Snce any attempt for the player to insert a move beyond the playable area is already caught during the
	  // parsing process, the own_square_exception translates into a simple else clause:
	if (destSq.hasPiece()) {
		if (destSq.pieceOwner() == getOpponent(inTurn)) {
			if (!results.capt)
				throw SquareValidationException("The destination square has an opponent's piece, yet no indication of a capture given.");
		}
		else
			throw SquareValidationException("The destination square is blocked by your own piece!");
	}
	else if (results.capt)
	{
		if (results.movedP != MoveId::P)
			throw SquareValidationException("The move is a capture move, but the destination square is empty.");
		// For pawn En Passant, special arrangements have to be made. These will be made during the actual move check.
	}

	
	// 2. find the source square. Also check if it can be uniquely identified.
	//    If it cannot be identified uniquely, or if it cannot be found at all,
	//    an error is thrown.
	//    Otherwise, mark the source coordinates accordingly:
	std::vector<std::shared_ptr<Piece>> matches;
	for (std::shared_ptr<Piece> const p : (*inTurn))
		if (p->getType() == results.movedP) {
			if (matchSrcSquare(p, results.src) && p->canMoveTo(results, board)) {
				matches.push_back(p);

				// If, at any point, there appears more than one possible piece which can
				// make the same move, the move analysis has to be abandoned completely.
				// The following check takes care of this:
				if (matches.size() > 1)
					throw IllegalMoveException("There are more than one piece that can make this move. Please, adjust your notation");
			}
		}

	if (matches.size() == 0)
		throw IllegalMoveException("No piece can make this move");
	else results.src = matches[0]->getCoords();
}

// printMoveLine: outputStream, separatorChar, padding, lineNum -> void
// Prints the given already-played move line into the outputStream in
// user-prefered format.
//
// The format is the following:
//
// "lineNum. whiteMove@@@@@@blackMove"
// where '@' is a placeholder for the chosen separator character, and as many of
// the separator characters will be printed as there is padding left after pushing
// the whiteMove-representation into the outputStream with AT LEAST one separator
// character in between the whiteMove and the blackMove string representations.
void GameManager::printMoveLine(std::ostream& out, char separator, int padding, int lineNum) const {
	int nextMove = (lineNum - 1) * 2;
	out << lineNum << ". ";
	
	if ((int)moves.size() > nextMove) {
		std::string next = moves[nextMove];
		out << next;
		padding -= next.size();
		do {
			out << separator;
		} while (--padding > 0);

		nextMove++;
		if ((int)moves.size() > nextMove)
			out << moves[nextMove];
	}
	
}

// handlePromotion: MoveAnalysisResults& -> void
// Handles the promotion of a pawn.
// Also sets the result's promotedPiece to the string representation of the
// promotion.
void GameManager::handlePromotion(MoveAnalysisResults& results) {
	int success = false;
	std::string& ans = results.promotionPiece;
	std::shared_ptr<Piece> oldP = board.getPiece(results.dest);
	std::shared_ptr<Piece> newP;

	std::cout << "Your pawn will be promoted! ";
	while (!success) {
		if (ans.empty()) {
			std::cout << "Select the promotion piece (R, N, B, Q): ";
			std::getline(std::cin, ans);
		}

		if (pieceMapper.find(ans[0]) == pieceMapper.end()) {
			std::cout << "[" << ans << "]: illegal piece symbol." << std::endl;
		}
		else {
			switch (pieceMapper[ans[0]]) {
				case (MoveId::R):
					newP = std::shared_ptr<Piece>(new Rook(results.dest, inTurn));
					success = true;
					break;
				case (MoveId::N):
					newP = std::shared_ptr<Piece>(new Knight(results.dest, inTurn));
					success = true;
					break;
				case (MoveId::B):
					newP = std::shared_ptr<Piece>(new Bishop(results.dest, inTurn));
					success = true;
					break;
				case (MoveId::Q):
					newP = std::shared_ptr<Piece>(new Queen(results.dest, inTurn));
					success = true;
					break;
				default:
					std::cout << "Illegal piece promotion." << std::endl;
					break;
			}
		}
	}

	initNewPiece(newP, inTurn);
	inTurn->removePiece(oldP);
}

// handleCastling: MoveId, testFlag -> void
// handles the castling move.
// The values below are mostly hardcoded for programming convenience.
// This solution is as ugly as is the castling move itself in chess.
//
// if testFlag is set, does not perform the castling. Instead returns
// true or false depending on whether the castling can be made.
//
// if castling cannot be performed and testFlag is not set,
// throws an IllegalMoveException.
bool GameManager::handleCastling(MoveId mP, bool test) {
	std::shared_ptr<Piece> king = inTurn->getKing();
	Player* opp = getOpponent(inTurn);

	// A LISP-style helper macro - see explanation below:
	#define if_with_test_flag(condition, ret, alt_ret) if (condition)        \
											               if (test)         \
												               return ret;   \
												           else              \
													           alt_ret
																									    // EXAMPLE / EXPLANATION:
	if_with_test_flag(king->moved_p(),																    // test for whether the king has already moved
				 	  false,																			// if king has moved and test-flag is set, return false
					  throw IllegalMoveException("Castling not possible. King has already moved."));	// else, if king has moved, throw an IllegalMoveException
																									    // if king hasn't moved already, do nothing.

	if_with_test_flag(threatensSquare(king->getCoords(), opp),
					  false,
					  throw IllegalMoveException("Castling not possible. King is under a check."));
	
	std::shared_ptr<Piece> rook;
	int castlingRank = king->getCoords().rank();

	// Two almost identical "mirror" branches are used instead of more generic automatization for not having to
	// introduce multiple nearly pointless variables.
	if (mP == MoveId::OO) {
		if_with_test_flag(!board.hasPiece(SquareCoords(7, castlingRank)) || board.squarePieceType(SquareCoords(7, castlingRank)) != MoveId::R,
						  false, 
						  throw IllegalMoveException("Castling not possible: Rook has already moved."));
		
		rook = board.getPiece(SquareCoords(7, castlingRank));
		if_with_test_flag(rook->getOwner() != inTurn || rook->moved_p(),
						  false,
						  throw IllegalMoveException("Castling not possible. Rook has already moved."));


		for (int file = 5; file < 7; file++)
		{
			SquareCoords nextSq(file, castlingRank);
			if_with_test_flag(board.hasPiece(nextSq) || threatensSquare(nextSq, opp),
							  false,
							  throw IllegalMoveException("Castling not possible. Problematic square at: (" + std::to_string(file) + ", " + std::to_string(castlingRank) + ")"));
		}

		if (!test) {
			board.removePiece(king);
			board.removePiece(rook);

			king->setCoords(SquareCoords(6, castlingRank));
			rook->setCoords(SquareCoords(5, castlingRank));

			board.setPiece(king);
			board.setPiece(rook);
		}
	}
	else {
		if_with_test_flag(!board.hasPiece(SquareCoords(0, castlingRank)) || board.squarePieceType(SquareCoords(0, castlingRank)) != MoveId::R,
						  false,
						  throw IllegalMoveException("Castling not possible: Rook has already moved."));

		rook = board.getPiece(SquareCoords(0, castlingRank));
		if_with_test_flag(rook->getOwner() != inTurn || rook->moved_p(),
						  false,
						  throw IllegalMoveException("Castling not possible: invalid rook. Rook has already moved"));

		for (int file = 3; file > 0; file--)
		{
			SquareCoords nextSq(file, castlingRank);
			if_with_test_flag(board.hasPiece(nextSq) || threatensSquare(nextSq, opp),
							  false,
							  throw IllegalMoveException("Castling not possible."));
		}

		if (!test) {
			board.removePiece(king);
			board.removePiece(rook);

			king->setCoords(SquareCoords(2, castlingRank));
			rook->setCoords(SquareCoords(3, castlingRank));

			board.setPiece(king);
			board.setPiece(rook);
			king->move();
			rook->move();
		}
	}

	return true;
}

// threatensSquare: const squareCoords&, ptr to Player -> bool
// Is used to check if any piece of the given player can threaten the given square
bool GameManager::threatensSquare(const SquareCoords& dest, Player* player) {
	int opponentDirection = getOpponentDirection(player);

	for (std::shared_ptr<Piece> oppP : *player) {
		if (oppP->threatensSquare(dest, opponentDirection, board))
			return true;
	}

	return false;
}

// canMove: ptr to Player
// Is used to check if the given player can make any more moves.
// Returns true if the move can be made, false otherwise.
bool GameManager::canMove(Player* player) {
	// the logic in a nutshell:
	// Piece-by-piece, produce a list of all the destination squares available for the given piece
	// ignore the legality of the moves for now.
	// Test all the destination squares for a each piece one by one, by using validateMove with test flag set to true.
	// if any move produces a legal chess state, return true.
	// Otherwise return false.
	int oppDir = getOpponentDirection(player);

	for (std::shared_ptr<Piece> piece : *player) {
		SquareCoords src = piece->getCoords();
		MoveAnalysisResults res;
		res.movedP = piece->getType();
		res.src = src;
		res.opponentDir = oppDir;

		std::vector<SquareCoords> destSquares;
		piece->reachableSquares(destSquares, oppDir, board);

		for (SquareCoords sq : destSquares) {
			res.dest = sq;
			res.captureCoords = sq;
			res.capt = false;
			res.enPassantMove = false;

			// For pawns, there exists the possibility of En Passant,
			// which complicates things considerably. Thus pawn capture has to be taken care of
			// separately:
			if (res.movedP == MoveId::P && !res.dest.sameFile(src)) {
				res.capt = true;
				if (!board.hasPiece(sq)) {
					res.enPassantMove = true;
					res.captureCoords.setCoords(sq.file(), src.rank());
				}
			} // For normal capture, the setup is very simple:
			else if (board.hasPiece(sq))
				res.capt = true;

			// Finally, the moveAnalysis is complete - see if the move can be validated:
			if (validateMove(res, player)) {
				return true;
			}
		}
	}

	// if normal moves don't work, castling won't work either,
	// therefore we can just return false:
	return false;
}

// finalizeGameState: MoveAnalysisResults& -> void
// Examines whether the game is a checkmate or a stalemate,
// updates the move string if the last move was a check or
// if the game is a checkmate and finally records the move
// into the game's move list.
void GameManager::finalizeGameState(MoveAnalysisResults& results) {
	const SquareCoords& kingCoords = getOpponent(inTurn)->getKing()->getCoords();
	int oppDir = getOpponentDirection(inTurn);
	std::vector<std::shared_ptr<Piece>> threatPieces;

	bool check = threatensSquare(kingCoords, inTurn);

	if (!canMove(getOpponent(inTurn))) {
		if (check) {
			results.checkmateMove = true;
			checkmate = true;
			lastMsg = inTurnPlayer() + " won!";
		}
		else {
			stalemate = true;
			lastMsg = "Stalemate.";
		}
	}
	else if (check)
		results.checkMove = true;

	mParser.addSpecialNotation(results);
	
	moves.push_back(results.move);

	if (!(checkmate || stalemate))
		changeTurn();
}

// canMakeMove: const MoveAnalysisResults& -> bool
// returns true if the given move (contained in MoveAnalysisResults) can be made, false otherwise.
//
// Requires that the results DS be set properly in advance - does not change it in any way.
// Note also that validateMove does not handle castling. Castling moves need to be checked
// separately.
//
bool GameManager::validateMove(const MoveAnalysisResults& results, Player* player) {
	// 1. Save the old game state for backtracking purposes:
	std::shared_ptr<Piece> movedP = board.getPiece(results.src);
	std::shared_ptr<Piece> capturedP = nullptr;
	
	// 2. Make the move:
	if (results.capt) {
		capturedP = board.getPiece(results.captureCoords);
		getOpponent(player)->removePiece(capturedP);
		board.removePiece(capturedP);
	}

	board.removePiece(movedP);
	movedP->setCoords(results.dest);
	board.setPiece(movedP);

	// The move can be taken if the current player's King is not in check after the move.
	// 3. Check if the current player's king is under check:
	bool success = true;
	const SquareCoords& kingCoords = player->getKing()->getCoords();
	if (threatensSquare(kingCoords, getOpponent(player)))
		success = false;
				
	// 4. backtrack:
	//
	// 4.1. Put back the player's piece:
	board.removePiece(movedP);
	movedP->setCoords(results.src);
	board.setPiece(movedP);
	// 4.2. put back the opponent's piece if necessary:
	if (results.capt) {
		getOpponent(player)->addPiece(capturedP);
		board.setPiece(capturedP);
	}

	// 5. Return whether the move was a success or a failure:
	return success;
}

// commitMove: std::string, MoveAnalysisResults& -> void
// Commits to the move analyzed in the results ds.
//
// Does not change the results ds in any way.
//
// Assumes that the move is legal.
// Only makes the move - does not finalize the move. Finalizing
// The move, as well as making sure that the move is legal, is
// the responsibility of the makeMove -method.
//
// Does not handle castling. Castling needs to be committed separately.
//
// May change the results DS only in case of a promotion move.
void GameManager::commitMove(MoveAnalysisResults& results) {
	std::shared_ptr<Piece> movedP = board.getPiece(results.src);
	std::shared_ptr<Piece> capturedP = nullptr;

	if (results.capt) {
		capturedP = board.getPiece(results.captureCoords);
		getOpponent(inTurn)->removePiece(capturedP);
		board.removePiece(capturedP);
	}

	board.removePiece(movedP);
	movedP->setCoords(results.dest);
	board.setPiece(movedP);

	if (results.enPassantThreat)
		movedP->setEnPassant();

	if (results.promotionMove)
		handlePromotion(results);

	movedP->move();
}

// Public methods:
// -----------------------------
GameManager::GameManager() {
	// Initialize the piece mapper used by both the game manager and the move parser:
	pieceMapper['R'] = MoveId::R;
	pieceMapper['N'] = MoveId::N;
	pieceMapper['B'] = MoveId::B;
	pieceMapper['Q'] = MoveId::Q;
	pieceMapper['K'] = MoveId::K;
	pieceMapper['a'] = MoveId::P;
	pieceMapper['b'] = MoveId::P;
	pieceMapper['c'] = MoveId::P;
	pieceMapper['d'] = MoveId::P;
	pieceMapper['e'] = MoveId::P;
	pieceMapper['f'] = MoveId::P;
	pieceMapper['g'] = MoveId::P;
	pieceMapper['h'] = MoveId::P;

	lastMsg = "";
	whiteName = "Red";
	blackName = "Blue";
	// Initialize the game:
	initGame();
}

// makeMove: string -> bool
//
// Tries to make a move with the given chess notation parser.
//
// If the move succeeds, changes the game state accordingly and returns true.
// Otherwise, records the error message and returns false without making the move.
// -----------------------------------------
bool GameManager::makeMove(std::string move) {
	if (checkmate || stalemate) {
		lastMsg = "The game has already ended: ";
		if (checkmate)
			lastMsg += inTurnPlayer() + "won!";
		else
			lastMsg += "Stalemate.";
		
		return false;
	}
	
	MoveAnalysisResults results;
	results.move = move;

	// Parse the given move:
	try {
		mParser.parseNewMove(results, pieceMapper);
		results.opponentDir = getOpponentDirection(inTurn);
	}
	catch (ParseException const& e) {
		lastMsg = e.what();
		return false;
	}

	// If the move was a castling move, handle it separately:
	MoveId mP = results.movedP;

	if (mP == MoveId::OO || mP == MoveId::OOO)
		try {
		handleCastling(mP);
		}
	    catch (IllegalMoveException const& e) {
			lastMsg = e.what();
			return false;
	    }
	else {
		// If the move was not a castling move, validate the source and destination
		// squares and find all the necessary move information:
		try {
			extractMove(results);
		}
		catch (IllegalMoveException const& e) {
			lastMsg = "[" + move + "]: " + e.what();
			return false;
		}
		catch (SquareValidationException const& e) {
			lastMsg = "[" + move + "]: " + e.what();
			return false;
		}

		// Finally, commit to it, if it can be taken:
		if (!validateMove(results, inTurn)) {
			lastMsg = "[" + move + "]: The move will leave you king under a check!";
			return false;
		}
		else
			commitMove(results);
	}

	// If either the castling branch or the normal move branch was successful,
	// finalize the move and signal success:
	finalizeGameState(results);
	return true;
}

// getErrorMsg: void -> std::string
// Returns the last GameManager message encountered:
const std::string& GameManager::getMsg() const {
	return lastMsg;
}

// The following algorithm prints the game board in the console.
// Each rank consists of three lines, and each file consists of
// five rows (=spaces), in order to make the board look nicer to the players.
//
// Each square can be either white or black, and if there is a piece on
// the square, it should be printed at the center of the square.
// For each square, the center of the square resides in the second line
// and third row.
//
// NOTE:
// printBoard changes the console attributes, and therefore is not portable.
void GameManager::printBoard() const {
	static const int linesPerRank = 3;
	static const int middleLine = 2;
	static const int rowsPerFile = 5;
	static const int middleRow = 3;
	static const unsigned char whiteSquareColor = 219;
	static const unsigned char blackSquareColor = ' ';
	int pmll = rankLim * linesPerRank;
	int printMoveLines = (std::min<int>)(turnNum, pmll);
	
	unsigned char currSquareColor = whiteSquareColor;

	// A helper macro for switching the current square between white and black:
    #define switchSquareColor()  if (currSquareColor == whiteSquareColor) \
	                                 currSquareColor = blackSquareColor;  \
	                             else                                     \
		                            currSquareColor = whiteSquareColor;

	for (int rank = 7; rank >= 0; rank--) {
		for (int line = 1; line <= linesPerRank; ++line, --pmll) {
			// 1. Print the left indentation for the line: 
			if (line == middleLine)
				std::cout << rank + 1 << ' ';
			else
				std::cout << "  ";

			// 2. Print the current line for the current rank:
			for (int file = 0; file < 8; ++file) {
				
				for (int row = 1; row <= rowsPerFile; ++row) {

					if (line == middleLine && row == middleRow) {
						Square sq = board.getSquare(SquareCoords(file, rank));
						if (sq.hasPiece()) {
							if (sq.pieceOwner() == &white)
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), whitePieceColor);
							else
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), blackPieceColor);
							
							sq.printPiece();
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), consoleColor);
						}
						else
							std::cout << currSquareColor;
					}
					else
						std::cout << currSquareColor;
				}
				switchSquareColor()
			}
			
			if (printMoveLines >= pmll) {
				std::cout << "  ";
				printMoveLine(std::cout, ' ', playerPadding, turnNum - printMoveLines + 1);
				printMoveLines--;
			}
			std::cout << std::endl;
		}
		switchSquareColor()
	}

	// 3. Print the file labels:
	std::cout << "  ";
	for (char file = 'a'; file < 'i'; file++)
		for (int row = 1; row <= rowsPerFile; ++row) {
			if (row == middleRow)
				std::cout << file;
			else
				std::cout << ' ';
		}

	std::cout << std::endl;
}

// inTurnPlayer:
// Returns the string representation of the player in turn.
std::string GameManager::inTurnPlayer() const {
	if (inTurn == &white)
		return whiteName;
	else
		return blackName;
}

// restart: void -> void
// Begins a new chess game.
// If the previous game was restarted abruptly, announces the winner.
void GameManager::restart() {
	// TODO: announce the winner if necessary.
	initGame();
}

// isCheckmate: void -> void
// returns true if the game has ended
// in a checkmate, false otherwise.
bool GameManager::isCheckmate() const {
	return checkmate;
}

// isStalemmate: void -> void
// returns true if the game has ended
// in a stalemate, false otherwise.
bool GameManager::isStalemate() const {
	return stalemate;
}

// save: filename -> bool
// Tries to save the game into the file specified by filename.
// If the saving succeeds, returns true, else records the
// error message into lastMsg and returns false.
//
// NOTE:
// if the game is finished, saves the game in a user friendly
// format from which the moves are easy to read.
//
// If the game is not finished yet, saves the moves in a format that
// allows for easy loading of the game afterwards.
bool GameManager::save(std::string filename) {
	bool success = false;
	std::ofstream savefile;
	
	savefile.open(filename, std::ios::trunc);

	if (savefile.is_open()) {

		if (checkmate || stalemate) {
			for (int line = 1; line <= turnNum; line++) {
				printMoveLine(savefile, ' ', playerPadding, line);
				savefile << std::endl;
			}
		}
		else {
			for (size_t moveNum = 0; moveNum < moves.size(); moveNum++)
				savefile << moves[moveNum] << std::endl;
		}

		savefile.close();
		success = true;
	}
	else lastMsg = "SAVE ERROR: could not open the file " + filename;

	return success;
}

// save: filename -> bool
// Tries to load a game from the file specified by filename.
// If the loading succeeds, returns true, else records the
// error message into lastMsg and returns false.
bool GameManager::load(std::string filename) {
	std::ifstream loadfile;
	std::string nextMove;
	bool done = false;

	loadfile.open(filename);
	if (loadfile.is_open()) {
		initGame();

		while (!done) {
			std::getline(loadfile, nextMove);
			if (nextMove.size() > 0) {

				if (!makeMove(nextMove)) {
					lastMsg = "[" + nextMove + "]: The move could not be made:";
					loadfile.close();
					return false;
				}

			}
			else done = true;
		}

		loadfile.close();
	}
	else lastMsg = "LOAD ERROR: could not open the file " + filename;

	return done;
}

// save: filename -> bool
// Tries to take back the specifired number of .
// If the saving succeeds, returns true, else records the
// error message into lastMsg and returns false.
bool GameManager::takeBack(size_t n) {
	bool success = false;

	if (n <= 0) {
		lastMsg = "ERROR: A non-positive argument given to takeback command.";
	}
	else if (moves.size() < n) {
		lastMsg = "ERROR: More takebacks than made moves.";
	}
	else {
		std::vector<std::string>::iterator first = moves.begin();
		std::vector<std::string>::iterator last = moves.end()-n;
		std::vector<std::string> moveList(first, last);
		initGame();

		for (std::string m : moveList)
			makeMove(m);

		success = true;
	}

	return success;
}