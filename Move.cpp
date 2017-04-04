#include "Move.h"
#include <ostream>

using std::ostream;

typedef std::pair<int, int> pos;

Move::Move(pos coordinate, int col, MoveType move) : coor_(coordinate), color_(col), move_(move) {}

pos Move::getCoor() const {
	return coor_;
}
int Move::getColor() const {
	return color_;
}

bool Move::isPass() const {
	return color_ == 0;
}
bool Move::isResign() const {
	return color_ == -1;
}

Move Move::move(pos coor, int color) {
	return { coor, color, MoveType::MOVE };
}
Move Move::pass() {
	return { { -1, -1 } , 0, MoveType::PASS };
}
Move Move::resign() {
	return { { -1, -1 } , -1, MoveType::RESIGN };
}

ostream& operator<<(ostream &s, const Move &m) {
	if (m.isResign()) {
		return s << "R";
	}

	if (m.isPass()) {
		return s << "P";
	}

	return s << m.getColor() << ": " << m.getCoor().first << ", " << m.getCoor().second;
}