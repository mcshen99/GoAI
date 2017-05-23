#pragma once

#include "MoveType.h"
#include <utility>

typedef std::pair<int, int> pos;

struct Move {
private:
	pos coor_;
	int color_;
	MoveType move_;

	Move(pos coordinate, int col, MoveType move);

public:
	pos getCoor() const;
	int getColor() const;

	bool isPass() const;
	bool isResign() const;

	static Move move(pos coor, int color);
	static Move pass(int color);
	static Move resign(int color);

	bool operator==(const Move& m) const;
};

namespace std {
	template<>
	struct hash<Move> {
		size_t operator()(const Move &move) const {
			int h = 0;
			if (move.isResign()) {
				h = -31;
			} else if (!move.isPass()) {
				h = move.getCoor().first * 31 * 31 + move.getCoor().second * 31 + move.getColor();
			}
			return h;
		}
	};
}
