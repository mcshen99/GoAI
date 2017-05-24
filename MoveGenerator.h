#pragma once

#include "Board.h"
#include "RandomPlayout.h"

class MoveGenerator {
private:
	pos current_;
	int count_;

	Board board_;
	int player_;
	
	static std::default_random_engine gen_;
	static std::uniform_int_distribution<int> dist_;

public:
	MoveGenerator(const Board& b, int player);

	std::pair<bool, Move> next();
	
	bool empty();
};