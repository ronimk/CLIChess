#pragma once
#include <vector>
#include <memory>

class Piece;

// The player class contains information of all of the player's pieces.
//
// The player's pieces can be iterated over with a simple for each loop:
// for (std::shared_ptr<Piece> p : Player) { /* Do something */ }
//
class Player
{
private:
	std::vector<std::shared_ptr<Piece>> ownedPieces;

public:
	void clear();
	void beginTurn() const;
	const std::shared_ptr<Piece> getKing() const;
	void addPiece(std::shared_ptr<Piece> p);
	void removePiece(std::shared_ptr<Piece> p);
	std::vector<std::shared_ptr<Piece>>::iterator begin() noexcept;
	std::vector<std::shared_ptr<Piece>>::iterator end() noexcept;
};

