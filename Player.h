#pragma once

#include <ostream>
#include <vector>
#include "Board.h"
#include <map>

class Player {
public:
	virtual std::ostream& comment(std::ostream& s) const = 0;
	virtual Move move(const Board& board, const std::vector<Move>& history) = 0;
};

std::ostream& operator<<(std::ostream& s, const Player& p);