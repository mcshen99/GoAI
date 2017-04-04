#pragma once

#include <ostream>
#include "Move.h"
#include "Board.h"

class Player {
public:
	virtual std::ostream& comment(std::ostream& s) const = 0;
	virtual Move move(const Board& board) = 0;
};

std::ostream& operator<<(std::ostream& s, const Player& p);