#pragma once

#include <vector>

#include "Board.h"
#include "RandomPlayout.h"
#include "PatternGenerator.h"

class MoveGenerator {
private:
	bool usePattern_;
	PatternGenerator pattern_;

	pos current_;
	int count_;

	const Board& board_;
	int color_;
	
	static std::default_random_engine gen_;
	static std::uniform_int_distribution<int> dist_;
	static std::uniform_real_distribution<double> real_;

public:
	MoveGenerator(const Board& b, int color, const std::vector<Move>& moves);

	std::pair<bool, Move> next();
	
	bool empty();
};