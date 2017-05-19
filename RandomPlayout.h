#pragma once

#include <random>
#include "Board.h"
#include <map>

class RandomPlayout {
private:
	const int dirs[2][4] = { { 1, -1, 0, 0 },{ 0, 0, 1, -1 } };

	std::default_random_engine gen_;

	std::uniform_real_distribution<double> dist_;

	std::vector<double> komi_;

	bool isGroup(const Board& board, const Move& m);

	bool isAtari(const Board& board, const Move& m);

	bool isCapture(const Board& board, const Move& m);

public:
	RandomPlayout(std::vector<double> komi);

	Move move(const Board& board, int player);

	int simulate(Board& board, int player);

	std::map<pos, double> gen_playout(const Board& board, int player);
};