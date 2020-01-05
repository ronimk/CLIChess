#include <ostream>
#include "CLIChessDefinitions.h"

// legalSquare_p: const squareCoords& -> bool
// A helper predicate function for ensuring that the given
// coordinates point to a valid square in the board.
bool legalSquare_p(const SquareCoords& coords) {
	return coords.file() >= 0 && coords.file() < fileLim &&
		coords.rank() >= 0 && coords.rank() < rankLim;
}

// SquareCoords Interface:
// -----------------------

std::ostream& operator<<(std::ostream& out, const SquareCoords& sc) {
	out << "(" << sc.pos.first << ", " << sc.pos.second << ")";
	return out;
}

SquareCoords::SquareCoords() {
	pos.first = -1;
	pos.second = -1;
}

SquareCoords::SquareCoords(int file, int rank) {
	pos.first = file;
	pos.second = rank;
}

SquareCoords::SquareCoords(const SquareCoords& copy) {
	pos = copy.pos;
}

SquareCoords& SquareCoords::operator=(const SquareCoords& rhs) {
	pos = rhs.pos;
	return *this;
}

bool SquareCoords::operator==(const SquareCoords& rhs) const {
	return pos == rhs.pos;
}

bool SquareCoords::operator!=(const SquareCoords& rhs) const {
	return pos != rhs.pos;
}


bool SquareCoords::sameFile(const SquareCoords& comp) const {
	return pos.first == comp.pos.first;
}
// It would have been much better to create new 'file==' and 'rank==' operators
// instead of sameFile and rankFile -methods (and even file= and rank= assignment
// operators), but it seemed a bit of an overkille.
//
// However, it would be a nice practice on template metaprogramming, 
// to modify the code accordingly in the future.
bool SquareCoords::sameRank(const SquareCoords& comp) const {
	return pos.second == comp.pos.second;
}

int SquareCoords::file() const {
	return pos.first;
}

int SquareCoords::rank() const {
	return pos.second;
}

void SquareCoords::setCoords(int file, int rank) {
	pos.first = file;
	pos.second = rank;
}