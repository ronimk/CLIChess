#include <algorithm>
#include "Player.h"
#include "Pieces.h"

// clear: void -> void
// Clears all the player's pieces. Is used to restart the game.
void Player::clear() {
	ownedPieces.clear();
}

// beginTurn: void -> void
// Initializes the game turn for the player.
void Player::beginTurn() const {
	for (std::shared_ptr<Piece> p : ownedPieces)
		p->reset();
}

// getKing: void -> const std::shared_ptr to Piece
// An easy way to retrieve the player's king.
const std::shared_ptr<Piece> Player::getKing() const {
	for (std::shared_ptr<Piece> p : ownedPieces)
		if (p->getType() == MoveId::K)
			return p;

	return nullptr;
}

// addPiece: std::shared_ptr<Piece> -> void
// Adds a piece into the player's piece inventory.
// Is used for game initialization, restarts and promotions.
void Player::addPiece(std::shared_ptr<Piece> p) {
	ownedPieces.push_back(p);
}

// removePiece: std::shared_ptr<Piece> -> void
// Removes a piece that matches the given piece,
// from the player's piece inventory.
void Player::removePiece(std::shared_ptr<Piece> p) {
	ownedPieces.erase(std::remove(ownedPieces.begin(), ownedPieces.end(), p), ownedPieces.end());
}

// being() and end() allow for a simple for each iteration over the player to find and collect all the pieces
// relevant to a single move, or for examining whether the opponent's king is in check - or checkmated.
std::vector<std::shared_ptr<Piece>>::iterator Player::begin() noexcept {
	return ownedPieces.begin();
};

std::vector<std::shared_ptr<Piece>>::iterator Player::end() noexcept {
	return ownedPieces.end();
};