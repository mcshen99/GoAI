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
	static Move pass();
	static Move resign();
};